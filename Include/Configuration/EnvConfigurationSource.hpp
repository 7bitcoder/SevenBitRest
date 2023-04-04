#pragma once

#include <memory>

#include "Common/EnvVariables.hpp"
#include "Common/Utils.hpp"
#include "Configuration/IConfigurationSource.hpp"

namespace sd
{
    class EnvConfigurationSource final : public IConfigurationSource
    {
      private:
        class Provider final : public IConfigurationProvider
        {
          private:
            EnvConfigurationSource &_source;
            Json::object_t _configuration;

          public:
            Provider(EnvConfigurationSource &source) : _source(source) {}

            void load()
            {
                _configuration.clear();
                for (auto &var : _source.getVariavles())
                {
                    auto &[key, value] = var;
                    utils::updateJsonWithValue(_configuration, key, value);
                }
            }

            Json::object_t get() { return _configuration; }
        };

      private:
        EnvVariables _envVariables;

      public:
        EnvConfigurationSource(std::string prefix = EnvironmentVariables::Prefix)
            : _envVariables(utils::getEnvVariables(prefix))
        {
        }

        IConfigurationProvider::Ptr build(IConfigurationBuilder &builder) { return std::make_unique<Provider>(*this); }

        EnvVariables getVariavles() { return _envVariables; }
    };
} // namespace sd