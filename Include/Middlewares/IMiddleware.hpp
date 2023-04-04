#pragma once

#include <memory>

#include "Engine/IContext.hpp"

namespace sd
{
    struct INextCallback
    {
        virtual void next() = 0;

        void operator()() { next(); }

        virtual ~INextCallback() = default;
    };

    struct IMiddleware
    {
        using Ptr = std::unique_ptr<IMiddleware>;

        virtual void next(IContext &ctx, INextCallback &callback) = 0;

        virtual ~IMiddleware() = default;
    };

} // namespace sd