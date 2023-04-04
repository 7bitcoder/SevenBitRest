#pragma once

#include <memory>
#include <type_traits>

#include "Common/Utils.hpp"
#include "Engine/IContext.hpp"
#include "Middlewares/IMiddleware.hpp"
#include "Middlewares/IMiddlewareCreator.hpp"

namespace sd
{
    template <class MiddlewareT> class MiddlewareCreator final : public IMiddlewareCreator
    {
      public:
        using Ptr = std::unique_ptr<MiddlewareCreator>;

        MiddlewareCreator()
        {
            static_assert(std::is_base_of_v<IMiddleware, MiddlewareT>, "Type T must inherit from IMiddleware");
        }

        IMiddleware::Ptr create(IContext &ctx)
        {
            if constexpr (std::is_default_constructible_v<MiddlewareT>)
            {
                return std::make_unique<MiddlewareT>();
            }
            else if constexpr (std::is_constructible_v<MiddlewareT, IContext &>)
            {
                return std::make_unique<MiddlewareT>(ctx);
            }
            else
            {
                static_assert(utils::notSupportedType<MiddlewareT>, "Middleware constructor should accept IContext&");
            }
        }
    };
} // namespace sd