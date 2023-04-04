#pragma once

#include "Engine/IContext.hpp"

namespace sd
{
    struct IContextAccessor
    {
        virtual IContext &get() = 0;

        virtual void set(IContext *context) = 0;

        virtual ~IContextAccessor() = default;
    };
} // namespace sd