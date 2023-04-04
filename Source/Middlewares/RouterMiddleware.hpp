#pragma once

#include <memory>
#include <unordered_map>

#include "Engine/IEndpoint.hpp"
#include "Http/HttpMethod.hpp"
#include "Http/IRequest.hpp"
#include "Middlewares/IMiddleware.hpp"
#include "Middlewares/MiddlewareCreator.hpp"
#include "Router/Router.hpp"

namespace sd
{

    class RouterMiddleware final : public IMiddleware
    {
      private:
        IRouter *_router;

      public:
        RouterMiddleware(IRouter *router) : _router(router) {}

        void next(IContext &ctx, INextCallback &callback) final
        {
            auto path = ctx.getRequest().getPath();
            auto method = ctx.getRequest().getMethod();
            auto endpoint = _router->match(method, path);

            ctx.getRoutingData().setEndpoint(endpoint);

            callback.next();
        }
    };

    class RouterMiddlewareCreator final : public IMiddlewareCreator
    {

      private:
        IRouter *_router;

      public:
        using Ptr = std::unique_ptr<RouterMiddlewareCreator>;

        RouterMiddlewareCreator(IRouter *router) : _router(router) {}

        IMiddleware::Ptr create(IContext &ctx) final { return std::make_unique<RouterMiddleware>(_router); }
    };
} // namespace sd