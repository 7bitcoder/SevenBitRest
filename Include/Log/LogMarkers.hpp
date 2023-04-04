#pragma once

#include <string_view>

#include "Log/LogLevel.hpp"

namespace sd
{
    class LogMarker
    {
      private:
        std::string_view _message;
        LogLevel _level;

      public:
        LogMarker(std::string_view message, LogLevel level) : _message(message), _level(level) {}

        std::string_view get() const { return _message; }
        LogLevel getLogLevel() const { return _level; }
    };

    class Trace final : public LogMarker
    {
      public:
        Trace(std::string_view message) : LogMarker(message, LogLevel::TraceLevel) {}
    };

    class Debug final : public LogMarker
    {
      public:
        Debug(std::string_view message) : LogMarker(message, LogLevel::DebugLevel) {}
    };

    class Info final : public LogMarker
    {
      public:
        Info(std::string_view message) : LogMarker(message, LogLevel::InformationLevel) {}
    };

    class Warning final : public LogMarker
    {
      public:
        Warning(std::string_view message) : LogMarker(message, LogLevel::WarningLevel) {}
    };

    class Error final : public LogMarker
    {
      public:
        Error(std::string_view message) : LogMarker(message, LogLevel::ErrorLevel) {}
    };

    class Critical final : public LogMarker
    {
      public:
        Critical(std::string_view message) : LogMarker(message, LogLevel::CriticalLevel) {}
    };
} // namespace sd