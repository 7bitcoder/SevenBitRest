#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW true

#include <algorithm>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/bind_cancellation_slot.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/parallel_group.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/make_unique.hpp>
#include <boost/optional.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Common/ServerSettings.hpp"
#include "Engine/BoostExtensions.hpp"
#include "Engine/CancellationSignals.hpp"
#include "Engine/CertLoader.hpp"
#include "Engine/Url.hpp"
#include "Log/ILogger.hpp"

namespace sd
{
    using executor_type = boost::asio::io_context::executor_type;
    using executor_with_default =
        boost::asio::as_tuple_t<boost::asio::use_awaitable_t<executor_type>>::executor_with_default<executor_type>;

    using NativeRequest = boost::beast::http::request<boost::beast::http::string_body>;
    using NativeRequestHeaders = NativeRequest::header_type;
    using NativeResponse = boost::beast::http::response<boost::beast::http::string_body>;
    using NativeResponseHeaders = NativeResponse::header_type;
    using NativeParamList = boost::beast::http::param_list;
    using ServerRequestHandler = std::function<NativeResponse(NativeRequest &)>;

    class BoostBeastServer
    {
      private:
        ILogger::Ptr _logger;
        ServerRequestHandler _handler;
        const ServerSettings _settings;
        CancellationSignals _cancellation;

      public:
        BoostBeastServer(ILogger &logger, ServerRequestHandler handler, ServerSettings settings)
            : _logger(logger.createFor<BoostBeastServer>()), _handler(handler), _settings(settings)
        {
        }

        int start(std::vector<Url> urls)
        {
            auto const threads = std::max<int>(1, _settings.threadsNumber);

            // The io_context is required for all I/O
            boost::asio::io_context ioc{threads};

            // The SSL context is required, and holds certificates
            boost::asio::ssl::context sslCtx{boost::asio::ssl::context::tlsv12};
            bool certLoaded = false;
            // This holds the self-signed certificate used by the server

            for (auto urlSettings : urls)
            {
                if (urlSettings.host == "localhost")
                {
                    urlSettings.host = "127.0.0.1";
                }
                auto const address = boost::asio::ip::make_address(urlSettings.host);
                auto const port = urlSettings.port;
                auto const useSsl = urlSettings.useSsl;

                if (useSsl && !certLoaded)
                {
                    certLoaded = true;
                    CertLoader::load(sslCtx);
                }

                // Create and launch a listening routine
                if (useSsl)
                {
                    boost::asio::co_spawn(
                        ioc, listen(sslCtx, boost::asio::ip::tcp::endpoint{address, port}, _cancellation),
                        boost::asio::bind_cancellation_slot(_cancellation.slot(), boost::asio::detached));
                }
                else
                {
                    boost::asio::co_spawn(
                        ioc, listen(ioc, boost::asio::ip::tcp::endpoint{address, port}, _cancellation),
                        boost::asio::bind_cancellation_slot(_cancellation.slot(), boost::asio::detached));
                }
            }

            // Capture SIGINT and SIGTERM to perform a clean shutdown
            boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&](boost::beast::error_code const &, int sig) {
                if (sig == SIGINT)
                    _cancellation.emit(boost::asio::cancellation_type::all);
                else
                {
                    // Stop the `io_context`. This will cause `run()`
                    // to return immediately, eventually destroying the
                    // `io_context` and all of the sockets in it.
                    ioc.stop();
                }
            });

            // Run the I/O service on the requested number of threads
            std::vector<std::thread> v;
            v.reserve(threads - 1);
            for (auto i = threads - 1; i > 0; --i)
                v.emplace_back([&ioc] { ioc.run(); });
            ioc.run();

            // (If we get here, it means we got a SIGINT or SIGTERM)

            // Block until all the threads exit
            for (auto &t : v)
                t.join();

            return EXIT_SUCCESS;
        }

        void stop() { _cancellation.emit(); }

      private:
        // Accepts incoming connections and launches the sessions.
        template <class Context>
        BOOST_ASIO_NODISCARD boost::asio::awaitable<void, executor_type> listen(Context &ctx,
                                                                                boost::asio::ip::tcp::endpoint endpoint,
                                                                                CancellationSignals &sig)
        {
            typename boost::asio::ip::tcp::acceptor::rebind_executor<executor_with_default>::other acceptor{
                co_await boost::asio::this_coro::executor};
            if (!initListener(acceptor, endpoint))
                co_return;

            while ((co_await boost::asio::this_coro::cancellation_state).cancelled() ==
                   boost::asio::cancellation_type::none)
            {
                auto [ec, sock] = co_await acceptor.async_accept();
                const auto exec = sock.get_executor();
                using stream_type = typename boost::beast::tcp_stream::rebind_executor<executor_with_default>::other;
                if (!ec)
                    // We dont't need a strand, since the awaitable is an implicit strand.
                    boost::asio::co_spawn(exec, detectSession(stream_type(std::move(sock)), ctx),
                                          boost::asio::bind_cancellation_slot(sig.slot(), boost::asio::detached));
            }
        }

        bool initListener(
            typename boost::asio::ip::tcp::acceptor::rebind_executor<executor_with_default>::other &acceptor,
            const boost::asio::ip::tcp::endpoint &endpoint)
        {
            boost::beast::error_code ec;
            // Open the acceptor
            acceptor.open(endpoint.protocol(), ec);
            if (ec)
            {
                fail(ec, "open");
                return false;
            }

            // Allow address reuse
            acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
            if (ec)
            {
                fail(ec, "set_option");
                return false;
            }

            // Bind to the server address
            acceptor.bind(endpoint, ec);
            if (ec)
            {
                fail(ec, "bind");
                return false;
            }

            // Start listening for connections
            acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec)
            {
                fail(ec, "listen");
                return false;
            }
            return true;
        }

        BOOST_ASIO_NODISCARD boost::asio::awaitable<void, executor_type> detectSession(
            typename boost::beast::tcp_stream::rebind_executor<executor_with_default>::other stream,
            boost::asio::io_context &ctx)
        {
            boost::beast::flat_buffer buffer;

            // Set the timeout.
            stream.expires_after(std::chrono::seconds(_settings.timeoutSec));
            // on_run
            co_await runSession(stream, buffer);
        }

        BOOST_ASIO_NODISCARD boost::asio::awaitable<void, executor_type> detectSession(
            typename boost::beast::tcp_stream::rebind_executor<executor_with_default>::other stream,
            boost::asio::ssl::context &ctx)
        {
            boost::beast::flat_buffer buffer;

            // Set the timeout.
            stream.expires_after(std::chrono::seconds(_settings.timeoutSec));
            // on_run
            auto [ec, result] = co_await boost::beast::async_detect_ssl(stream, buffer);
            // on_detect
            if (ec)
                co_return fail(ec, "detect");

            if (result)
            {
                using stream_type = typename boost::beast::tcp_stream::rebind_executor<executor_with_default>::other;
                boost::beast::ssl_stream<stream_type> ssl_stream{std::move(stream), ctx};

                auto [ec, bytes_used] = co_await ssl_stream.async_handshake(
                    boost::asio::ssl::stream_base::server, buffer.data(),
                    boost::asio::as_tuple(boost::asio::use_awaitable_t<executor_type>{}));

                if (ec)
                    co_return fail(ec, "handshake");

                buffer.consume(bytes_used);
                co_await runSession(ssl_stream, buffer);
            }
            else
            {
                co_await do_eof(stream);
            }
        }

        template <typename Stream>
        BOOST_ASIO_NODISCARD boost::asio::awaitable<void, executor_type> runSession(Stream &stream,
                                                                                    boost::beast::flat_buffer &buffer)
        {
            boost::beast::http::request_parser<boost::beast::http::string_body> parser;
            // Apply a reasonable limit to the allowed size
            // of the body in bytes to prevent abuse.
            parser.body_limit(_settings.bodyLimit);

            auto [ec, bytes_transferred] = co_await boost::beast::http::async_read(stream, buffer, parser);

            if (ec == boost::beast::http::error::end_of_stream)
                co_await do_eof(stream);

            if (ec)
                co_return fail(ec, "read");

            // this can be
            // while ((co_await boost::asio::this_coro::cancellation_state).cancelled() ==
            // boost::asio::cancellation_type::none) on most compilers
            for (auto cs = co_await boost::asio::this_coro::cancellation_state;
                 cs.cancelled() == boost::asio::cancellation_type::none;
                 cs = co_await boost::asio::this_coro::cancellation_state)
            {
                // if (boost::beast::websocket::is_upgrade(parser.get()))
                // {
                //     // Disable the timeout.
                //     // The boost::beast::websocket::stream uses its own timeout settings.
                //     boost::beast::get_lowest_layer(stream).expires_never();

                //     co_await runWebsocketSession(stream, buffer, parser.release());
                //     co_return;
                // }

                // we follow a different strategy then the other example: instead of queue responses,
                // we always to one read & write in parallel.
                auto res = parser.release();
                boost::beast::http::message_generator msg = _handler(res);
                // if (!msg.keep_alive())
                // {
                auto [ec, sz] = co_await boost::beast::async_write(stream, std::move(msg));
                if (ec)
                    fail(ec, "write");
                co_return;
                // }

                // auto [_, ec_r, sz_r, ec_w, sz_w] =
                //     co_await boost::asio::experimental::make_parallel_group(
                //         boost::beast::http::async_read(stream, buffer, parser, boost::asio::deferred),
                //         boost::beast::async_write(stream, std::move(msg), boost::asio::deferred))
                //         .async_wait(boost::asio::experimental::wait_for_all(),
                //                     boost::asio::as_tuple(boost::asio::use_awaitable_t<executor_type>{}));

                // if (ec_r)
                //     co_return fail(ec_r, "read");

                // if (ec_w)
                //     co_return fail(ec_w, "write");
            }
        }

        template <typename Stream, typename Body, typename Allocator>
        BOOST_ASIO_NODISCARD boost::asio::awaitable<void, executor_type> runWebsocketSession(
            Stream &stream, boost::beast::flat_buffer &buffer,
            boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
        {
            boost::beast::websocket::stream<Stream &> ws{stream};

            // Set suggested timeout settings for the websocket
            ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

            // Set a decorator to change the Server of the handshake
            ws.set_option(
                boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type &res) {
                    res.set(boost::beast::http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) + " advanced-server-flex");
                }));

            // Accept the websocket handshake
            auto [ec] = co_await ws.async_accept(req);
            if (ec)
                co_return fail(ec, "accept");

            while (true)
            {

                // Read a message
                std::size_t bytes_transferred = 0u;
                std::tie(ec, bytes_transferred) = co_await ws.async_read(buffer);

                // This indicates that the websocket_session was closed
                if (ec == boost::beast::websocket::error::closed)
                    co_return;
                if (ec)
                    co_return fail(ec, "read");

                ws.text(ws.got_text());
                std::tie(ec, bytes_transferred) = co_await ws.async_write(buffer.data());

                if (ec)
                    co_return fail(ec, "write");

                // Clear the buffer
                buffer.consume(buffer.size());
            }
        }

        // Report a failure
        void fail(boost::beast::error_code ec, char const *what)
        {
            // boost::asio::ssl::error::stream_truncated, also known as an SSL "short read",
            // indicates the peer closed the connection without performing the
            // required closing handshake (for example, Google does this to
            // improve performance). Generally this can be a security issue,
            // but if your communication protocol is self-terminated (as
            // it is with both HTTP and WebSocket) then you may simply
            // ignore the lack of close_notify.
            //
            // https://github.com/boostorg/beast/issues/38
            //
            // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
            //
            // When a short read would cut off the end of an HTTP message,
            // Beast returns the error boost::beast::boost::beast::http::error::partial_message.
            // Therefore, if we see a short read here, it has occurred
            // after the message has been completed, so it is safe to ignore it.

            if (ec == boost::asio::ssl::error::stream_truncated)
                return;

            _logger->logError(std::string{what} + ": " + ec.message());
        }

        template <typename Stream> boost::asio::awaitable<void, executor_type> do_eof(Stream &stream)
        {
            boost::beast::error_code ec;
            stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            co_return;
        }

        template <typename Stream>
        BOOST_ASIO_NODISCARD boost::asio::awaitable<void, executor_type> do_eof(
            boost::beast::ssl_stream<Stream> &stream)
        {
            co_await stream.async_shutdown(boost::asio::use_awaitable_t<executor_type>{});
        }
    };
} // namespace sd