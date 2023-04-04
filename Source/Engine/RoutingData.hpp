#pragma once

#include <memory>
#include <stdexcept>
#include <variant>

#include "BoostBeastServer.hpp"
#include "DI/ServiceProvider.hpp"
#include "Engine/IContext.hpp"
#include "Engine/IRoutingData.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/RouteParams.hpp"

namespace sd
{

    class RoutingData final : public IRoutingData
    {
      private:
        Request &_request;

        const IEndpoint *_endpoint = nullptr;

        mutable RouteParams::Ptr _routeParams;

      public:
        RoutingData(Request &native) : _request(native) {}

        const IEndpoint *getEndpoint() const { return _endpoint; }

        void setEndpoint(const IEndpoint *endpoint) { _endpoint = endpoint; }

        const IRouteParamsView &getRouteParams() const
        {
            if (!_routeParams)
            {
                _routeParams = createRouteParams(_request.getPath());
            }
            return *_routeParams;
        }

        ~RoutingData() = default;

      private:
        RouteParams::Ptr createRouteParams(std::string_view path) const
        {
            auto params = std::make_unique<RouteParams>();
            if (!_endpoint)
            {
                return params;
            }
            auto &endpointSegments = _endpoint->getRouteTemplateSegments();
            auto segmentsStr = utils::split(path);
            if (segmentsStr.empty())
            {
                // todo throw
            }
            segmentsStr.erase(segmentsStr.begin());
            if (endpointSegments.size() != segmentsStr.size())
            {
                return params;
            }
            for (int i = 0; i < endpointSegments.size(); ++i)
            {
                auto &templateSegment = endpointSegments.at(i);
                auto &segment = segmentsStr.at(i);
                if (auto routeParam = std::get_if<RouteParam>(&templateSegment))
                {
                    params->add(routeParam->name, segment);
                }
            }
            return params;
        }
    };
} // namespace sd