#pragma once

#include <deque>
#include <exception>
#include <iostream>

#include "Engine/IContext.hpp"
#include "Middlewares/IMiddleware.hpp"
#include "Middlewares/MiddlewareCreator.hpp"
#include "Middlewares/MiddlewareCreators.hpp"

namespace sd
{
    class MiddlewaresRunner final : INextCallback
    {
      private:
        IContext &_ctx;
        MiddlewareCreators::Iterator _current;
        MiddlewareCreators::Iterator _end;

      public:
        MiddlewaresRunner(IContext &ctx, const MiddlewareCreators &creators)
            : _ctx(ctx), _current(creators.begin()), _end(creators.end())
        {
        }
        void run() { next(); }

        ~MiddlewaresRunner() = default;

      private:
        void next() final
        {
            if (auto middleware = getNextMiddleware())
            {
                middleware->next(_ctx, *this);
            }
        }

        IMiddleware::Ptr getNextMiddleware()
        {
            if (_current == _end)
            {
                return nullptr;
            }
            if (const auto &middlewareCreator = *(_current++))
            {
                return middlewareCreator->create(_ctx);
            }
            // TODO throw
            return nullptr;
        }
    };
} // namespace sd