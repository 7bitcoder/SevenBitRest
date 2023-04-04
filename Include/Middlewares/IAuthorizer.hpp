#pragma once

#include <memory>
#include <string>

namespace sd
{
    struct IContext;

    struct AuthorizationResult
    {
        bool isAuthorized = false;
        std::string reason = "";
    };

    struct IAuthorizer
    {
        using Ptr = std::unique_ptr<IAuthorizer>;

        virtual AuthorizationResult authorize(IContext &ctx) const = 0;

        virtual ~IAuthorizer() = default;
    };

} // namespace sd