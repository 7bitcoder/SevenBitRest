#pragma once

#include <string_view>

#include "Claims/IIdentity.hpp"

namespace sd
{
    struct IPrincipal
    {
        using Ptr = std::unique_ptr<IPrincipal>;

        virtual IIdentity &getIdentity() = 0;

        virtual bool isInRole(std::string_view role) const = 0;

        virtual ~IPrincipal() = default;
    };
} // namespace sd