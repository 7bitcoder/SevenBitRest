#pragma once

#include <thread>

#include "Common/ServerSettings.hpp"
#include "Configuration/IConfiguration.hpp"
#include "Services/IServerSettingsProvider.hpp"

namespace sd
{
    class ServerSettingsProvider final : public IServerSettingsProvider
    {
      private:
        static inline const std::string ThreadsNumber = "threadsNumber";
        static inline const std::string Url = "url";
        static inline const std::string DefaultUrl = "http://localhost:9090";

        IConfiguration &_configuration;
        ServerSettings _settings;

      public:
        ServerSettingsProvider(IConfiguration *configuration, ServerSettings settings)
            : _configuration(*configuration), _settings(settings)
        {
            checkSettings();
        }

        ServerSettings getSettings() const { return _settings; }

      private:
        void checkSettings()
        {
            setUrls();
            setThreadsNumber();
        }

        void setUrls()
        {
            if (_settings.urls.empty())
            {
                _settings.urls = tryGetUrlFromConfig();
            }
            if (_settings.urls.empty())
            {
                _settings.urls = {DefaultUrl};
            }
        }

        void setThreadsNumber()
        {
            if (_settings.threadsNumber < 1)
            {
                _settings.threadsNumber = tryGetThreadsNumberFromConfig();
            }
            if (_settings.threadsNumber < 1)
            {
                _settings.threadsNumber = std::max(1u, std::thread::hardware_concurrency());
            }
        }

        std::vector<std::string> tryGetUrlFromConfig()
        {
            std::vector<std::string> result;
            if (auto urls = _configuration.find(Url))
            {
                if (urls->is_array())
                {
                    for (auto url : urls->get_array())
                    {
                        result.push_back(urls->get_string());
                    }
                }
                else
                {
                    result.push_back(urls->get_string());
                }
            }
            return result;
        }

        uint16_t tryGetThreadsNumberFromConfig()
        {
            if (auto threadsNumber = _configuration.find(ThreadsNumber))
            {
                if (threadsNumber->is_number())
                {
                    return static_cast<int>(threadsNumber->get_signed());
                }
                auto str = threadsNumber->get_string();
                return std::atoi(str.c_str());
            }
            return 0;
        }
    };
} // namespace sd