#pragma once

#include <memory>

#include "Common/Utils.hpp"
#include "Engine/IContext.hpp"
#include "Middlewares/IMiddleware.hpp"

namespace sd
{
    struct IMiddlewareCreator
    {
        using Ptr = std::unique_ptr<IMiddlewareCreator>;

        virtual IMiddleware::Ptr create(IContext &ctx) = 0;

        virtual ~IMiddlewareCreator() = default;
    };
} // namespace sd