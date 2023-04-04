#pragma once
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "Common/Utils.hpp"
#include "Engine/Action.hpp"
#include "Engine/IEndpoint.hpp"
#include "Middlewares/IAuthorizer.hpp"

namespace sd
{

    class Endpoint final : public IEndpoint
    {
      private:
        std::string _pathTemplate;
        HttpMethod _method;
        Action _action;
        std::vector<IAuthorizer::Ptr> _authorizers;
        RouteTemplateSegments _routeTemplateSegments;

      public:
        Endpoint(HttpMethod method, std::string_view path, Action action)
            : _method(method), _pathTemplate(path), _action(std::move(action)), _routeTemplateSegments(_pathTemplate)
        {
        }

        void addPreTask() {}

        void addAuthorization(IAuthorizer::Ptr authorizer) { _authorizers.push_back(std::move(authorizer)); }

        const RouteTemplateSegments &getRouteTemplateSegments() const { return _routeTemplateSegments; }

        void addPostTask() {}

        HttpMethod getHttpMethod() const { return _method; }

        std::string_view getRouteTemplate() const { return _pathTemplate; }

        void executeAction(IContext &ctx) const { _action(ctx); }

        const std::vector<IAuthorizer::Ptr> &getAuthorization() const { return _authorizers; }

        ~Endpoint() = default;
    };
} // namespace sd