#pragma once

#include "Middlewares/IMiddleware.hpp"
#include "Middlewares/MiddlewareCreator.hpp"
#include "Middlewares/MiddlewareLambda.hpp"

namespace sd
{
    template <class Lambda> class MiddlewareLambdaCreator final : public IMiddlewareCreator
    {
      private:
        Lambda _lambda;

      public:
        MiddlewareLambdaCreator(Lambda lambda) : _lambda(lambda) {}

        IMiddleware::Ptr create(IContext &ctx) { return std::make_unique<MiddlewareLambda<Lambda>>(_lambda); }
    };
} // namespace sd