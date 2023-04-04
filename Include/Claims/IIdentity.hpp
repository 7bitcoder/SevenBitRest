#pragma once

#include <optional>
#include <string_view>

namespace sd
{
    struct IIdentity
    {
        using Ptr = std::unique_ptr<IIdentity>;

        virtual std::optional<std::string> getAuthenticationType() const = 0;

        virtual bool isAuthenticated() const = 0;

        virtual std::optional<std::string> getName() const = 0;

        virtual ~IIdentity() = default;
    };
} // namespace sd