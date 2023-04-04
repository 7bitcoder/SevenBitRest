#pragma once

#include <string>

namespace sd
{
    enum LogLevel
    {
        TraceLevel = 0,
        DebugLevel = 1,
        InformationLevel = 2,
        WarningLevel = 3,
        ErrorLevel = 4,
        CriticalLevel = 5,
        None = 6
    };

    inline std::string logLevelToLogString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::TraceLevel:
            return "trce: ";
        case LogLevel::DebugLevel:
            return "dbug: ";
        case LogLevel::InformationLevel:
            return "info: ";
        case LogLevel::WarningLevel:
            return "warn: ";
        case LogLevel::ErrorLevel:
            return "fail: ";
        case LogLevel::CriticalLevel:
            return "crit: ";
        default:
            return "none: ";
        }
    }

    inline LogLevel logLevelFromString(std::string_view level)
    {
        if (level == "Trace")
        {
            return LogLevel::TraceLevel;
        }
        if (level == "Debug")
        {
            return LogLevel::DebugLevel;
        }
        if (level == "Information" || level == "Info")
        {
            return LogLevel::InformationLevel;
        }
        if (level == "Warning")
        {
            return LogLevel::WarningLevel;
        }
        if (level == "Error")
        {
            return LogLevel::ErrorLevel;
        }
        if (level == "Critical")
        {
            return LogLevel::CriticalLevel;
        }
        return LogLevel::None;
    }
} // namespace sd