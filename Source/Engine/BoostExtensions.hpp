//
// Copyright (c) 2022 Seth Heeren (sgheeren at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include <boost/asio/async_result.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/detail/type_traits.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/span.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/serializer.hpp>
#include <boost/core/span.hpp>
#include <boost/smart_ptr/make_unique.hpp>
#include <boost/throw_exception.hpp>
#include <memory>
#include <type_traits>

namespace boost
{
    namespace beast
    {

/** Determine if type satisfies the <em>BuffersGenerator</em> requirements.

    This metafunction is used to determine if the specified type meets the
    requirements for a buffers generator.

    The static member `value` will evaluate to `true` if so, `false` otherwise.

    @tparam T a type to check
*/
#ifdef BOOST_BEAST_DOXYGEN
        template <class T> struct is_buffers_generator : integral_constant<bool, automatically_determined>
        {
        };
#else
        template <class T, class = void> struct is_buffers_generator : std::false_type
        {
        };

        template <class T>
        struct is_buffers_generator<T,
                                    detail::void_t<decltype(bool(std::declval<T const &>().is_done()),
                                                            typename T::const_buffers_type(std::declval<T &>().prepare(
                                                                std::declval<error_code &>())),
                                                            std::declval<T &>().consume(std::size_t{}))>>
            : std::true_type
        {
        };
#endif

        /** Write all output from a BuffersGenerator to a stream.

            This function is used to write all of the buffers generated
            by a caller-provided BuffersGenerator to a stream. The call
            will block until one of the following conditions is true:

            @li A call to the generator's `is_done` returns `false`.

            @li An error occurs.

            This operation is implemented in terms of one or more calls
            to the stream's `write_some` function.

            @param stream The stream to which the data is to be written.
            The type must support the <em>SyncWriteStream</em> concept.

            @param generator The generator to use.

            @param ec Set to the error, if any occurred.

            @return The number of bytes written to the stream.

            @see BuffersGenerator
        */
        template <class SyncWriteStream, class BuffersGenerator
#if !BOOST_BEAST_DOXYGEN
                  ,
                  typename std::enable_if<
                      is_buffers_generator<typename std::decay<BuffersGenerator>::type>::value>::type * = nullptr
#endif
                  >
        std::size_t write(SyncWriteStream &stream, BuffersGenerator &&generator, beast::error_code &ec);

        /** Write all output from a BuffersGenerator to a stream.

            This function is used to write all of the buffers generated
            by a caller-provided BuffersGenerator to a stream. The call
            will block until one of the following conditions is true:

            @li A call to the generator's `is_done` returns `false`.

            @li An error occurs.

            This operation is implemented in terms of one or more calls
            to the stream's `write_some` function.

            @param stream The stream to which the data is to be written.
            The type must support the <em>SyncWriteStream</em> concept.

            @param generator The generator to use.

            @return The number of bytes written to the stream.

            @throws system_error Thrown on failure.

            @see BuffersGenerator
        */
        template <class SyncWriteStream, class BuffersGenerator
#if !BOOST_BEAST_DOXYGEN
                  ,
                  typename std::enable_if<
                      is_buffers_generator<typename std::decay<BuffersGenerator>::type>::value>::type * = nullptr
#endif
                  >
        std::size_t write(SyncWriteStream &stream, BuffersGenerator &&generator);

        /** Write all output from a BuffersGenerator asynchronously to a
            stream.

            This function is used to write all of the buffers generated
            by a caller-provided `BuffersGenerator` to a stream. The
            function call always returns immediately. The asynchronous
            operation will continue until one of the following
            conditions is true:

            @li A call to the generator's `is_done` returns `false`.

            @li An error occurs.

            This operation is implemented in terms of zero or more calls
            to the stream's `async_write_some` function, and is known as
            a <em>composed operation</em>.  The program must ensure that
            the stream performs no other writes until this operation
            completes.

            @param stream The stream to which the data is to be written.
            The type must support the <em>SyncWriteStream</em> concept.

            @param generator The generator to use.

            @param token The completion handler to invoke when the
            operation completes. The implementation takes ownership of
            the handler by performing a decay-copy. The equivalent
            function signature of the handler must be:
            @code
            void handler(
                error_code const& error,        // result of operation
                std::size_t bytes_transferred   // the number of bytes written to the stream
            );
            @endcode
            Regardless of whether the asynchronous operation completes
            immediately or not, the handler will not be invoked from
            within this function. Invocation of the handler will be
            performed in a manner equivalent to using `net::post`.

            @see BuffersGenerator
        */
        template <
            class AsyncWriteStream, class BuffersGenerator,
            BOOST_BEAST_ASYNC_TPARAM2 CompletionToken = net::default_completion_token_t<executor_type<AsyncWriteStream>>
#if !BOOST_BEAST_DOXYGEN
            ,
            typename std::enable_if<is_buffers_generator<BuffersGenerator>::value>::type * = nullptr
#endif
            >
        BOOST_BEAST_ASYNC_RESULT2(CompletionToken)
        async_write(AsyncWriteStream &stream, BuffersGenerator generator,
                    CompletionToken &&token = net::default_completion_token_t<executor_type<AsyncWriteStream>>{});

        namespace detail
        {
            template <class AsyncWriteStream, class BuffersGenerator>
            struct write_buffers_generator_op : boost::asio::coroutine
            {
                write_buffers_generator_op(AsyncWriteStream &s, BuffersGenerator g) : s_(s), g_(std::move(g)) {}

                template <class Self> void operator()(Self &self, error_code ec = {}, std::size_t n = 0)
                {
                    BOOST_ASIO_CORO_REENTER(*this)
                    {
                        while (!g_.is_done())
                        {
                            BOOST_ASIO_CORO_YIELD
                            {
                                auto cb = g_.prepare(ec);
                                if (ec)
                                    goto complete;
                                s_.async_write_some(cb, std::move(self));
                            }
                            if (ec)
                                goto complete;

                            g_.consume(n);

                            total_ += n;
                        }

                    complete:
                        self.complete(ec, total_);
                    }
                }

              private:
                AsyncWriteStream &s_;
                BuffersGenerator g_;
                std::size_t total_ = 0;
            };

        } // namespace detail

        template <class SyncWriteStream, class BuffersGenerator,
                  typename std::enable_if<                                                              //
                      is_buffers_generator<typename std::decay<BuffersGenerator>::type>::value>::type * /*= nullptr*/
                  >
        size_t write(SyncWriteStream &stream, BuffersGenerator &&generator, beast::error_code &ec)
        {
            static_assert(is_sync_write_stream<SyncWriteStream>::value, "SyncWriteStream type requirements not met");

            ec.clear();
            size_t total = 0;
            while (!generator.is_done())
            {
                auto cb = generator.prepare(ec);
                if (ec)
                    break;

                size_t n = net::write(stream, cb, ec);

                if (ec)
                    break;

                generator.consume(n);
                total += n;
            }

            return total;
        }

        //----------------------------------------------------------

        template <class SyncWriteStream, class BuffersGenerator,
                  typename std::enable_if<
                      is_buffers_generator<typename std::decay<BuffersGenerator>::type>::value>::type * /*= nullptr*/
                  >
        std::size_t write(SyncWriteStream &stream, BuffersGenerator &&generator)
        {
            static_assert(is_sync_write_stream<SyncWriteStream>::value, "SyncWriteStream type requirements not met");
            beast::error_code ec;
            std::size_t n = write(stream, std::forward<BuffersGenerator>(generator), ec);
            if (ec)
                BOOST_THROW_EXCEPTION(system_error{ec});
            return n;
        }

        //----------------------------------------------------------

        template <class AsyncWriteStream, class BuffersGenerator, BOOST_BEAST_ASYNC_TPARAM2 CompletionToken,
                  typename std::enable_if<is_buffers_generator<BuffersGenerator>::value>::type * /*= nullptr*/
                  >
        BOOST_BEAST_ASYNC_RESULT2(CompletionToken)
        async_write(AsyncWriteStream &stream, BuffersGenerator generator, CompletionToken &&token)
        {
            static_assert(beast::is_async_write_stream<AsyncWriteStream>::value,
                          "AsyncWriteStream type requirements not met");

            return net::async_compose< //
                CompletionToken, void(error_code, std::size_t)>(
                detail::write_buffers_generator_op<AsyncWriteStream, BuffersGenerator>{stream, std::move(generator)},
                token, stream);
        }

        namespace http
        {

            /** Type-erased buffers generator for @ref http::message

                Implements the BuffersGenerator concept for any concrete instance of the
                @ref http::message template.

                @ref http::message_generator takes ownership of a message on construction,
                erasing the concrete type from the interface.

                This makes it practical for use in server applications to implement request
                handling:

                @code
                template <class Body, class Fields>
                http::message_generator handle_request(
                    string_view doc_root,
                    http::request<Body, Fields>&& request);
                @endcode

                The @ref beast::write and @ref beast::async_write operations are provided
                for BuffersGenerator. The @ref http::message::keep_alive property is made
                available for use after writing the message.
            */
            class message_generator
            {
              public:
                template <class T, std::size_t E = boost::dynamic_extent> using span = boost::span<T, E>;
                using const_buffers_type = span<net::const_buffer>;

                template <bool isRequest, class Body, class Fields>
                message_generator(http::message<isRequest, Body, Fields> &&);

                /// `BuffersGenerator`
                bool is_done() const { return impl_->is_done(); }

                /// `BuffersGenerator`
                const_buffers_type prepare(error_code &ec) { return impl_->prepare(ec); }

                /// `BuffersGenerator`
                void consume(std::size_t n) { impl_->consume(n); }

                /// Returns the result of `m.keep_alive()` on the underlying message
                bool keep_alive() const noexcept { return impl_->keep_alive(); }

              private:
                struct impl_base
                {
                    virtual ~impl_base() = default;
                    virtual bool is_done() = 0;
                    virtual const_buffers_type prepare(error_code &ec) = 0;
                    virtual void consume(std::size_t n) = 0;
                    virtual bool keep_alive() const noexcept = 0;
                };

                std::unique_ptr<impl_base> impl_;

                template <bool isRequest, class Body, class Fields> struct generator_impl;
            };

            template <bool isRequest, class Body, class Fields>
            message_generator::message_generator(http::message<isRequest, Body, Fields> &&m)
                : impl_(boost::make_unique<generator_impl<isRequest, Body, Fields>>(std::move(m)))
            {
            }

            template <bool isRequest, class Body, class Fields>
            struct message_generator::generator_impl : message_generator::impl_base
            {
                explicit generator_impl(http::message<isRequest, Body, Fields> &&m) : m_(std::move(m)), sr_(m_) {}

                bool is_done() override { return sr_.is_done(); }

                const_buffers_type prepare(error_code &ec) override
                {
                    sr_.next(ec, visit{*this});
                    return current_;
                }

                void consume(std::size_t n) override { sr_.consume((std::min)(n, beast::buffer_bytes(current_))); }

                bool keep_alive() const noexcept override { return m_.keep_alive(); }

              private:
                static constexpr unsigned max_fixed_bufs = 12;

                http::message<isRequest, Body, Fields> m_;
                http::serializer<isRequest, Body, Fields> sr_;

                std::array<net::const_buffer, max_fixed_bufs> bs_;
                const_buffers_type current_ = bs_; // subspan

                struct visit
                {
                    generator_impl &self_;

                    template <class ConstBufferSequence>
                    void operator()(error_code &, ConstBufferSequence const &buffers)
                    {
                        auto &s = self_.bs_;
                        auto &cur = self_.current_;

                        auto it = net::buffer_sequence_begin(buffers);

                        std::size_t n = std::distance(it, net::buffer_sequence_end(buffers));

                        n = (std::min)(s.size(), n);

                        cur = {s.data(), n};

                        std::copy_n(it, n, cur.begin());
                    }
                };
            };

        } // namespace http

    } // namespace beast
} // namespace boost
