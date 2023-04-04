#pragma once

#include <deque>
#include <memory>

#include "Engine/IContext.hpp"
#include "Middlewares/EndpointsMiddleware.hpp"
#include "Middlewares/IMiddleware.hpp"
#include "Middlewares/MiddlewareCreator.hpp"
#include "Middlewares/RouterMiddleware.hpp"
#include "Router/Router.hpp"

namespace sd
{
    class MiddlewareCreators
    {
      private:
        std::deque<IMiddlewareCreator::Ptr> _middlewareCreators;

      public:
        using Iterator = std::deque<IMiddlewareCreator::Ptr>::const_iterator;

        void add(IMiddlewareCreator::Ptr middleware) { _middlewareCreators.push_back(std::move(middleware)); }

        void addFront(IMiddlewareCreator::Ptr middleware) { _middlewareCreators.push_front(std::move(middleware)); }

        template <class MiddlewareCreator> bool has() const
        {
            for (auto &creator : _middlewareCreators)
            {
                if (dynamic_cast<MiddlewareCreator *>(creator.get()))
                {
                    return true;
                }
            }
            return false;
        }

        auto begin() const { return _middlewareCreators.begin(); }

        auto end() const { return _middlewareCreators.end(); }

        ~MiddlewareCreators() = default;
    };
} // namespace sd