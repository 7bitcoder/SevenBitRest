#pragma once

#include <memory>
#include <optional>
#include <string>
#include <tao/json/forward.hpp>

#include "Common/Utils.hpp"
#include "DI/ServiceProvider.hpp"
#include "Engine/FromBody.hpp"
#include "Engine/FromHeader.hpp"
#include "Engine/FromQuery.hpp"
#include "Engine/FromRoute.hpp"
#include "Engine/FromServices.hpp"
#include "Engine/FromServicesAll.hpp"
#include "Engine/IContext.hpp"
#include "Engine/IEndpoint.hpp"
#include "Engine/IWebApplicationEngine.hpp"
#include "Http/HttpMethod.hpp"
#include "Http/IRequest.hpp"
#include "Http/IResponse.hpp"
#include "Http/IResult.hpp"
#include "Http/Results.hpp"
#include "Middlewares/IMiddleware.hpp"
#include "Middlewares/MiddlewareCreator.hpp"
#include "Middlewares/MiddlewareLambdaCreator.hpp"
#include "Middlewares/SetupMiddleware.hpp"
#include "Router/Constrains.hpp"
#include "Router/IRouter.hpp"
#include "Services/Environment.hpp"
#include "Services/IContextAccessor.hpp"

namespace sd
{
    class WebApplicationBuilder;

    template <class T> T getArg(IContext &ctx);

    class WebApplication final
    {
      private:
        friend WebApplicationBuilder;

        IWebApplicationEngine::Ptr _engine;

        WebApplication(EngineDependencies::Ptr dependencies)
        {
            _engine = IWebApplicationEngine::createWebApplicationEngine(std::move(dependencies));
        }

      public:
        WebApplication() = delete;
        WebApplication(const WebApplication &) = delete;
        WebApplication(WebApplication &&) = default;
        WebApplication &operator=(const WebApplication &) = delete;
        WebApplication &operator=(WebApplication &&) = delete;

        template <class Lambda> IEndpoint *mapGet(std::string_view path, Lambda &&action)
        {
            return _engine->map(HttpMethod::Get, path, createAction(action, &Lambda::operator()));
        }

        template <class Lambda> IEndpoint *mapPut(std::string_view path, Lambda &&action)
        {
            return _engine->map(HttpMethod::Put, createAction(action, &Lambda::operator()));
        }

        template <class Lambda> IEndpoint *mapPatch(std::string_view path, Lambda &&action)
        {
            return _engine->map(HttpMethod::Patch, createAction(action, &Lambda::operator()));
        }

        template <class Lambda> IEndpoint *mapPost(std::string_view path, Lambda &&action)
        {
            return _engine->map(HttpMethod::Post, createAction(action, &Lambda::operator()));
        }

        template <class Lambda> IEndpoint *mapDelete(std::string_view path, Lambda &&action)
        {
            return _engine->map(HttpMethod::Delete, createAction(action, &Lambda::operator()));
        }

        template <class Lambda> IEndpoint *mapHead(std::string_view path, Lambda &&action)
        {
            return _engine->map(HttpMethod::Head, createAction(action, &Lambda::operator()));
        }

        void run(std::optional<std::string> url = std::nullopt, int threadsNumber = -1)
        {
            init();
            _engine->run(url, threadsNumber);
        }

        void stop() { _engine->stop(); }

        void useRouter() { _engine->useRouter(); }

        void useEndpoints() { _engine->useEndpoints(); }

        template <class MiddlewareT> void use() { _engine->use(std::make_unique<MiddlewareCreator<MiddlewareT>>()); }

        template <class Lambda> void use(Lambda lambda)
        {
            _engine->use(std::make_unique<MiddlewareLambdaCreator<Lambda>>(lambda));
        }

        const IConfiguration &getConfiguration() { return _engine->getConfiguration(); }

        const IEnvironment &getEnvironment() { return _engine->getEnvironment(); }

        ILogger &getLogger() { return _engine->getLogger(); }

        ServiceProvider &getServiceProvider() { return _engine->getServiceProvider(); }

        ~WebApplication() = default;

      private:
        void init()
        {
            _engine->init();
            _engine->useAsFirst(std::make_unique<MiddlewareCreator<SetupMiddleware>>());
        }

        template <class Lambda, class... Args> auto createAction(Lambda lambda, std::string (Lambda::*)(Args...) const)
        {
            return [lambda = std::move(lambda)](IContext &ctx) {
                TextResult textResult{lambda(getArg<Args>(ctx)...), "text/plain; charset=utf-8"};
                textResult.execute(ctx.getResponse());
            };
        }

        template <class Lambda, class... Args> auto createAction(Lambda lambda, IResult::Ptr (Lambda::*)(Args...) const)
        {
            return [lambda = std::move(lambda)](IContext &ctx) {
                if (IResult::Ptr result = lambda(getArg<Args>(ctx)...))
                {
                    result->execute(ctx.getResponse());
                }
                // todo throw exception null
            };
        }

        template <class Lambda, class Res, class... Args>
        auto createAction(Lambda lambda, Res (Lambda::*)(Args...) const)
        {
            return [lambda = std::move(lambda)](IContext &ctx) {
                OkResult okResult{lambda(getArg<Args>(ctx)...)};
                okResult.execute(ctx.getResponse());
            };
        }
    };

    template <class T> T getArg(IContext &ctx)
    {
        if constexpr (std::is_base_of_v<BaseParam, T>)
        {
            return T{ctx};
        }
        else
        {
            return getService<T>(ctx.getRequestServices());
        }
    }

    template <> inline IContext &getArg(IContext &ctx) { return ctx; }
    template <> inline const IContext &getArg(IContext &ctx) { return ctx; }
    template <> inline IContext *getArg(IContext &ctx) { return &ctx; }
    template <> inline IContext *const getArg(IContext &ctx) { return &ctx; }
    template <> inline const IContext *getArg(IContext &ctx) { return &ctx; }
    template <> inline const IContext *const getArg(IContext &ctx) { return &ctx; }
}; // namespace sd