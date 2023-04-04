#pragma once

#include <memory>

#include "Common/Json.hpp"

namespace sd
{
    struct IConfigurationProvider
    {
        using Ptr = std::unique_ptr<IConfigurationProvider>;

        virtual void load() = 0;

        virtual Json::object_t get() = 0;

        virtual ~IConfigurationProvider() = default;
    };
} // namespace sd