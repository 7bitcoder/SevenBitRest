#pragma once

#include "BoostBeastServer.hpp"
#include "Claims/ClaimsPrincipal.hpp"
#include "Common/Utils.hpp"
#include "DI/ServiceProvider.hpp"
#include "Data/DataContainer.hpp"
#include "Engine/IContext.hpp"
#include "Engine/RoutingData.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include <stdexcept>

namespace sd
{

    class Context : public IContext
    {
      private:
        Request _request;
        Response _response;

        IDataContainer::Ptr _dataContainer;
        IRoutingData::Ptr _routingData;
        ClaimsPrincipal::Ptr _user;
        ServiceProvider::Ptr _serviceProvider;

      public:
        Context(NativeRequest &native) : _request(native), _response(_request) {}

        ServiceProvider &getRequestServices() { return utils::getRequired(_serviceProvider); }

        void setServiceProvider(ServiceProvider::Ptr serviceProvider) { _serviceProvider = std::move(serviceProvider); }

        const IRequest &getRequest() const { return _request; }

        IResponse &getResponse() { return _response; }

        IRoutingData &getRoutingData() { return utils::getRequired(_routingData); }

        void setRoutingData(IRoutingData::Ptr routingData) { _routingData = std::move(routingData); }

        IDataContainer &getData() { return utils::getRequired(_dataContainer); }

        void setData(IDataContainer::Ptr data) { _dataContainer = std::move(data); }

        ClaimsPrincipal &getUser() { return utils::getRequired(_user); }

        void setUser(ClaimsPrincipal::Ptr user) { _user = std::move(user); }

        NativeResponse getNativeResponse() { return _response.getNative(); }

        ~Context() = default;
    };
} // namespace sd