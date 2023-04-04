#pragma once

#include <memory>

#include "Common/CliVariables.hpp"
#include "Common/Utils.hpp"
#include "Configuration/IConfigurationSource.hpp"

namespace sd
{
    class JsonConfigurationSource final : public IConfigurationSource
    {
      private:
        class Provider final : public IConfigurationProvider
        {
          private:
            JsonConfigurationSource &_source;
            Json::object_t _configuration;

          public:
            Provider(JsonConfigurationSource &source) : _source(source) {}

            void load() { _configuration = _source.getConfiguration(); }

            Json::object_t get() { return _configuration; }
        };

      private:
        Json::object_t _configuration;

      public:
        JsonConfigurationSource(Json::object_t configuration) : _configuration(std::move(configuration)) {}

        IConfigurationProvider::Ptr build(IConfigurationBuilder &builder) { return std::make_unique<Provider>(*this); }

        Json::object_t &getConfiguration() { return _configuration; }
    };
} // namespace sd