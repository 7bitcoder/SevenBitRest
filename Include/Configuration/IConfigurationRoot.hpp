#pragma once

#include <memory>
#include <vector>

#include "Common/Json.hpp"
#include "Configuration/IConfigurationProvider.hpp"

namespace sd
{
    struct IConfigurationRoot
    {
        using Ptr = std::unique_ptr<IConfigurationRoot>;

        virtual void reload() = 0;

        virtual std::vector<IConfigurationProvider::Ptr> &getProviders() = 0;

        virtual ~IConfigurationRoot() = default;
    };
} // namespace sd