#pragma once

#include "Common/ServerSettings.hpp"

namespace sd
{
    struct IServerSettingsProvider
    {
        virtual ServerSettings getSettings() const = 0;

        virtual ~IServerSettingsProvider() = default;
    };
} // namespace sd