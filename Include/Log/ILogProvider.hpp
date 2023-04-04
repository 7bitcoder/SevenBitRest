#pragma once

#include <string_view>

#include "Log/LogLevel.hpp"

namespace sd
{
    struct ILogProvider
    {
        virtual void log(LogLevel level, std::string_view message) = 0;

        virtual LogLevel getLogLevel() const = 0;

        virtual ~ILogProvider() = default;
    };
} // namespace sd