#pragma once

#include <iostream>
#include <string_view>

#include "Configuration/IConfiguration.hpp"
#include "Log/ILogProvider.hpp"
#include "Log/LogLevel.hpp"

namespace sd
{
    class LogProviderBase : public ILogProvider
    {
      protected:
        LogLevel _logLevel = LogLevel::InformationLevel;
        std::string _providerName;

      public:
        LogProviderBase(IConfiguration *config, std::string providerName) : _providerName(providerName)
        {
            _logLevel = getLogLevelFromConfig(config);
        }

        void log(LogLevel level, std::string_view message) final
        {
            if (level >= _logLevel && level != LogLevel::None)
            {
                logImpl(level, message);
            }
        }

        LogLevel getLogLevel() const final { return _logLevel; }

      protected:
        virtual void logImpl(LogLevel level, std::string_view message) = 0;

      private:
        LogLevel getLogLevelFromConfig(IConfiguration *config)
        {
            if (!config)
            {
                return LogLevel::InformationLevel;
            }

            if (auto logging = config->find("Logging"))
            {
                if (auto provider = logging->find(_providerName))
                {
                    if (auto logLevel = provider->find("LogLevel"))
                    {
                        return logLevelFromString(logLevel->get_string());
                    }
                }
                if (auto defaultLogLevel = logging->find("LogLevel"))
                {
                    return logLevelFromString(defaultLogLevel->get_string());
                }
            }
            return LogLevel::InformationLevel;
        }
    };
} // namespace sd