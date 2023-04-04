#pragma once

#include <string_view>

#include "Log/LogMarkers.hpp"

namespace sd
{
    struct ILogger
    {
        using Ptr = std::unique_ptr<ILogger>;

        template <class TService> std::unique_ptr<ILogger> createFor() { return createNamed(typeid(TService).name()); }

        virtual std::unique_ptr<ILogger> createNamed(std::string name) = 0;

        virtual ILogger &log(LogLevel level, std::string_view message) = 0;
        virtual ILogger &log(const LogMarker &marker) = 0;

        virtual ILogger &logTrace(std::string_view message) = 0;
        virtual ILogger &logDebug(std::string_view message) = 0;
        virtual ILogger &logInfo(std::string_view message) = 0;
        virtual ILogger &logWarning(std::string_view message) = 0;
        virtual ILogger &logError(std::string_view message) = 0;
        virtual ILogger &logCritical(std::string_view message) = 0;

        ILogger &operator<<(std::string_view message) { return logInfo(message); }
        ILogger &operator<<(const LogMarker &marker) { return log(marker); }

        virtual ~ILogger() = default;
    };
} // namespace sd