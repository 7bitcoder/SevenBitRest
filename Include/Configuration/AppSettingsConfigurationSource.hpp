#pragma once

#include <filesystem>
#include <memory>
#include <tao/json/type.hpp>

#include "Common/Utils.hpp"
#include "Configuration/IConfigurationSource.hpp"

namespace sd
{
    class AppSettingsConfigurationSource final : public IConfigurationSource
    {
      private:
        class Provider final : public IConfigurationProvider
        {
          private:
            AppSettingsConfigurationSource &_source;
            Json::object_t _configuration;

          public:
            Provider(AppSettingsConfigurationSource &source) : _source(source) {}

            void load()
            {
                _configuration.clear();
                if (auto config = tryGetJsonFromFile("appsettings.json"); config.is_object())
                {
                    _configuration.merge(config.get_object());
                }
                if (auto config = tryGetJsonFromFile("appsettings." + _source.getEnvName() + ".json");
                    config.is_object())
                {
                    _configuration.merge(config.get_object());
                }
            }

            Json::object_t get() { return _configuration; }

            ~Provider() = default;

          private:
            Json tryGetJsonFromFile(std::string filename)
            {
                if (std::filesystem::exists(filename))
                {
                    return tao::json::basic_from_file<JsonTraits>(filename);
                }
                return tao::json::empty_object;
            }
        };

      private:
        std::string _envName;

      public:
        AppSettingsConfigurationSource(std::string envName) : _envName(envName) {}

        IConfigurationProvider::Ptr build(IConfigurationBuilder &builder) { return std::make_unique<Provider>(*this); }

        const std::string &getEnvName() const { return _envName; }
    };
} // namespace sd