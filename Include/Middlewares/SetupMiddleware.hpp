#pragma once

#include "DI/ServiceProvider.hpp"
#include "Engine/IContext.hpp"
#include "Middlewares/IMiddleware.hpp"
#include "Services/IContextAccessor.hpp"

namespace sd
{

    class SetupMiddleware final : public IMiddleware
    {
      public:
        void next(IContext &ctx, INextCallback &next) final
        {
            setup(ctx);
            next();
        }

      private:
        void setup(IContext &ctx) { setupContextAccesor(ctx); }

        void setupContextAccesor(IContext &ctx)
        {
            if (auto accesor = ctx.getRequestServices().getService<IContextAccessor>())
            {
                accesor->set(&ctx);
            }
        }
    };
} // namespace sd