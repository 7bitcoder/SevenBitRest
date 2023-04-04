#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Http/HttpMethod.hpp"
#include "Router/RouteTemplateSegments.hpp"

namespace sd
{

    struct IContext;
    struct IAuthorizer;

    struct IEndpoint
    {
        using Ptr = std::unique_ptr<IEndpoint>;

        virtual void addPreTask() = 0;

        virtual void addAuthorization(std::unique_ptr<IAuthorizer> authorizer) = 0;

        virtual void addPostTask() = 0;

        virtual HttpMethod getHttpMethod() const = 0;

        virtual std::string_view getRouteTemplate() const = 0;

        virtual const RouteTemplateSegments &getRouteTemplateSegments() const = 0;

        virtual void executeAction(IContext &ctx) const = 0;

        virtual const std::vector<std::unique_ptr<IAuthorizer>> &getAuthorization() const = 0;

        virtual ~IEndpoint() = default;
    };
} // namespace sd
