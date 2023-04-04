#pragma once

#include "Middlewares/IMiddleware.hpp"

namespace sd
{
    template <class Lambda> class MiddlewareLambda final : public IMiddleware
    {
      private:
        Lambda _lambda;

      public:
        MiddlewareLambda(Lambda lambda) : _lambda(lambda) {}

        void next(IContext &ctx, INextCallback &next) { _lambda(ctx, next); }
    };
} // namespace sd