#pragma once

#include <memory>

#include "Engine/IEndpoint.hpp"
#include "Middlewares/IMiddleware.hpp"
#include "Middlewares/MiddlewareCreator.hpp"

namespace sd
{
    class EndpointsMiddleware : public IMiddleware
    {
      public:
        void next(IContext &ctx, INextCallback &callback)
        {
            if (auto endpoint = ctx.getRoutingData().getEndpoint())
            {
                return endpoint->executeAction(ctx);
            }
            callback.next();
        }
    };

    class EndpointsMiddlewareCreator final : public IMiddlewareCreator
    {
      public:
        IMiddleware::Ptr create(IContext &ctx) final { return std::make_unique<EndpointsMiddleware>(); }
    };
} // namespace sd