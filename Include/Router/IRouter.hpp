#pragma once

#include <memory>
#include <unordered_map>

#include "Engine/IEndpoint.hpp"
#include "Http/IRequest.hpp"
#include "Middlewares/IMiddleware.hpp"

namespace sd
{

    struct IRouter
    {
        using Ptr = std::unique_ptr<IRouter>;

        virtual IEndpoint *addEndpoint(IEndpoint::Ptr endpoint) = 0;

        virtual void compile() = 0;

        virtual const IEndpoint *match(HttpMethod method, std::string_view path) const = 0;

        virtual ~IRouter() = default;
    };
} // namespace sd