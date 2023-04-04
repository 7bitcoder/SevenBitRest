#pragma once

#include <_types/_uint16_t.h>
#include <boost/url/url.hpp>
#include <experimental/coroutine>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <typeindex>
#include <utility>
#include <vector>

#include "Common/LibraryConfig.hpp"
#include "Configuration/IConfiguration.hpp"
#include "DI/IServiceHolder.hpp"
#include "DI/ServiceOwner.hpp"
#include "DI/ServiceProvider.hpp"
#include "Engine/BoostBeastServer.hpp"
#include "Engine/CancellationSignals.hpp"
#include "Engine/Context.hpp"
#include "Engine/Endpoint.hpp"
#include "Engine/IContext.hpp"
#include "Engine/IWebApplicationEngine.hpp"
#include "Http/HttpMethod.hpp"
#include "Http/IResult.hpp"
#include "Log/ILogger.hpp"
#include "Log/LogMarkers.hpp"
#include "Middlewares/MiddlewareCreators.hpp"
#include "Middlewares/MiddlewaresRunner.hpp"
#include "Middlewares/RouterMiddleware.hpp"
#include "Router/Router.hpp"
#include "Services/ContextAccessor.hpp"
#include "Services/IContextAccessor.hpp"

namespace sd
{
    class WebApplicationEngine final : public IWebApplicationEngine
    {
      private:
        EngineDependencies::Ptr _dependencies;

        ILogger::Ptr _logger;
        MiddlewareCreators _middlewareCreators;

        BoostBeastServer _server;

      public:
        WebApplicationEngine(EngineDependencies::Ptr dependencies)
            : _dependencies(moveAndCheck(std::move(dependencies))),
              _logger(_dependencies->getLogger().createFor<WebApplicationEngine>()),
              _server(getThisLogger(), createHandler(), getServerSettings())
        {
        }

        static EngineDependencies::Ptr moveAndCheck(EngineDependencies::Ptr dependencies)
        {
            if (!dependencies)
            {
                throw std::runtime_error("dependencies is null");
            }
            return dependencies;
        }

        void init() final
        {
            getRouter().compile();
            getServiceProvider().prebuildSingeletons();
            checkMiddlewares();
        }

        void run(std::optional<std::string> url, int threadsNumber) final
        {
            auto urls = createUrls(url);

            getThisLogger() << "7BitRest framework version: " SEVENBITREST_VERSION;
            for (auto &url : urls)
            {
                getThisLogger() << "Application is running on url: " + url.toString();
            }

            _server.start(urls);
        }

        void stop() final { _server.stop(); }

        IEndpoint *map(HttpMethod method, std::string_view path, Action action) final
        {
            return addEndpoint(method, path, std::move(action));
        }

        void use(IMiddlewareCreator::Ptr creator) final { _middlewareCreators.add(std::move(creator)); }

        void useAsFirst(IMiddlewareCreator::Ptr creator) final { _middlewareCreators.addFront(std::move(creator)); }

        void useRouter() final { use(std::make_unique<RouterMiddlewareCreator>(&getRouter())); }

        void useEndpoints() final { use(std::make_unique<EndpointsMiddlewareCreator>()); }

        const IConfiguration &getConfiguration() final { return _dependencies->getConfiguration(); }

        const IEnvironment &getEnvironment() final { return _dependencies->getEnvironment(); }

        ILogger &getLogger() final { return _dependencies->getLogger(); }

        ServiceProvider &getServiceProvider() final { return _dependencies->getServiceProvider(); }

        ~WebApplicationEngine() {}

      private:
        ServerRequestHandler createHandler()
        {
            return [this](NativeRequest &req) { return handleRequest(req); };
        }

        ILogger &getThisLogger() { return *_logger; }

        ServerSettings getServerSettings() { return _dependencies->getSettingsProvider().getSettings(); }

        IRouter &getRouter() { return _dependencies->getRouter(); }

        IEndpoint *addEndpoint(HttpMethod method, std::string_view path, Action action)
        {
            auto endpoint = std::unique_ptr<Endpoint>(new Endpoint(method, path, std::move(action)));
            return getRouter().addEndpoint(std::move(endpoint));
        }

        void checkMiddlewares()
        {
            if (!_middlewareCreators.has<RouterMiddlewareCreator>())
            {
                useAsFirst(std::make_unique<RouterMiddlewareCreator>(&getRouter()));
            }
            if (!_middlewareCreators.has<EndpointsMiddlewareCreator>())
            {
                useEndpoints();
            }
        }

        NativeResponse handleRequest(NativeRequest &req)
        {
            try
            {
                Context context{req};
                context.setServiceProvider(getServiceProvider().createScoped());

                runMiddlewaresChain(context);

                return context.getNativeResponse();
            }
            catch (std::exception &e)
            {
                getThisLogger() << Error{e.what()};
            }
            catch (...)
            {
                getThisLogger() << Error{"Unknown exception occurred while processing request"};
            }
            boost::beast::http::response<boost::beast::http::string_body> res{
                boost::beast::http::status::internal_server_error, req.version()};
            return res;
        }

        void runMiddlewaresChain(IContext &ctx) const
        {
            MiddlewaresRunner runner{ctx, _middlewareCreators};
            runner.run();
        }

        std::vector<Url> createUrls(std::optional<std::string> url)
        {
            std::vector<Url> result;
            auto urls = getServerSettings().urls;
            if (url)
            {
                urls = {*url};
            }
            for (auto &urlString : urls)
            {
                result.emplace_back(urlString);
            }
            std::unordered_set<Url> unique(result.begin(), result.end());
            return {unique.begin(), unique.end()};
        }
    };
} // namespace sd