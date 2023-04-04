#pragma once

#include <memory>

#include "Claims/ClaimsPrincipal.hpp"
#include "Data/IDataContainer.hpp"
#include "Engine/IRoutingData.hpp"

namespace sd
{
    struct IRequest;
    struct IResponse;
    struct IEndpoint;
    class ServiceProvider;

    struct IContext
    {
        using Ptr = std::unique_ptr<IContext>;

        // Connection

        // Features

        virtual IDataContainer &getData() = 0;

        virtual void setData(IDataContainer::Ptr data) = 0;

        virtual const IRequest &getRequest() const = 0;

        // RequestAborted

        virtual ServiceProvider &getRequestServices() = 0;

        // virtual void setServiceProvider(ServiceProvider::Ptr serviceProvider) = 0;

        virtual IResponse &getResponse() = 0;

        // Session

        virtual IRoutingData &getRoutingData() = 0;

        virtual void setRoutingData(IRoutingData::Ptr data) = 0;

        // TraceIdentifier

        virtual ClaimsPrincipal &getUser() = 0;

        virtual void setUser(ClaimsPrincipal::Ptr user) = 0;

        // WebSockets

        virtual ~IContext() = default;
    };
} // namespace sd