#pragma once

#include <memory>

#include "Common/CliVariables.hpp"
#include "Common/Utils.hpp"
#include "Configuration/IConfigurationSource.hpp"

namespace sd
{
    class CliConfigurationSource final : public IConfigurationSource
    {
      private:
        class Provider final : public IConfigurationProvider
        {
          private:
            CliConfigurationSource &_source;
            Json::object_t _configuration;

          public:
            Provider(CliConfigurationSource &source) : _source(source) {}

            void load()
            {
                _configuration.clear();
                for (auto &var : _source.getVariables())
                {
                    auto &[key, value] = var;
                    utils::updateJsonWithValue(_configuration, key, value);
                }
            }

            Json::object_t get() { return _configuration; }
        };

      private:
        CliVariables _cliVariables;

      public:
        CliConfigurationSource(int argc, char **argv) : _cliVariables(utils::getCliVariables(argc, argv)) {}

        IConfigurationProvider::Ptr build(IConfigurationBuilder &builder) { return std::make_unique<Provider>(*this); }

        CliVariables &getVariables() { return _cliVariables; }
    };
} // namespace sd