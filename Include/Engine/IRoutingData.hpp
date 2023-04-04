#pragma once

#include <memory>

#include "Engine/IEndpoint.hpp"
#include "Http/IRouteParamsView.hpp"

namespace sd
{
    struct IRoutingData
    {
        using Ptr = std::unique_ptr<IRoutingData>;

        virtual const IEndpoint *getEndpoint() const = 0;

        virtual void setEndpoint(const IEndpoint *endpoint) = 0;

        virtual const IRouteParamsView &getRouteParams() const = 0;

        virtual ~IRoutingData() = default;
    };
} // namespace sd