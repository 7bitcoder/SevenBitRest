#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Common/Json.hpp"
#include "Common/Utils.hpp"
#include "Configuration/IConfiguration.hpp"
#include "Log/ILogProvider.hpp"
#include "Log/ILogger.hpp"
#include "Log/LogLevel.hpp"

namespace sd
{
    class Logger final : public ILogger
    {
      private:
        std::string _name = "Default";
        std::vector<ILogProvider *> _providers;
        LogLevel _logLevel;

      public:
        Logger(std::vector<ILogProvider *> provider, IConfiguration *config) : _providers(provider) { setLogLevel(); }

        std::unique_ptr<ILogger> createNamed(std::string name)
        {
            auto named = std::make_unique<Logger>(_providers, nullptr);
            named->_name = utils::demangle(name);
            named->_logLevel = _logLevel;
            return named;
        }

        ILogger &log(LogLevel level, std::string_view message)
        {
            if (level < _logLevel)
            {
                return *this;
            }
            auto finalMessage = logLevelToLogString(level);
            finalMessage.reserve(finalMessage.size() + _name.size() + message.size() + 8);
            finalMessage += _name;
            finalMessage += "\n      ";
            finalMessage += std::string{message};
            finalMessage += '\n';
            return logToProviders(level, finalMessage);
        }

        ILogger &log(const LogMarker &marker) { return log(marker.getLogLevel(), marker.get()); }

        ILogger &logTrace(std::string_view message) { return log(LogLevel::TraceLevel, message); }
        ILogger &logDebug(std::string_view message) { return log(LogLevel::DebugLevel, message); }
        ILogger &logInfo(std::string_view message) { return log(LogLevel::InformationLevel, message); }
        ILogger &logWarning(std::string_view message) { return log(LogLevel::WarningLevel, message); }
        ILogger &logError(std::string_view message) { return log(LogLevel::ErrorLevel, message); }
        ILogger &logCritical(std::string_view message) { return log(LogLevel::CriticalLevel, message); }

      private:
        ILogger &logToProviders(LogLevel level, std::string_view message)
        {
            for (auto &provider : _providers)
            {
                provider->log(level, message);
            }
            return *this;
        }

        void setLogLevel()
        {
            _logLevel = LogLevel::InformationLevel;
            for (auto &provider : _providers)
            {
                if (_logLevel > provider->getLogLevel())
                {
                    _logLevel = provider->getLogLevel();
                }
            }
        }
    };
} // namespace sd