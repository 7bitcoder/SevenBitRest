#pragma once

#include "Common/Json.hpp"
#include "Configuration/IConfigurationProvider.hpp"

namespace sd
{
    struct IConfigurationBuilder;

    struct IConfigurationSource
    {
        using Ptr = std::unique_ptr<IConfigurationSource>;

        virtual IConfigurationProvider::Ptr build(IConfigurationBuilder &builder) = 0;

        virtual ~IConfigurationSource() = default;
    };
} // namespace sd