#pragma once

#include <memory>
#include <tao/json/type.hpp>

#include "Common/Json.hpp"
#include "Configuration/IConfiguration.hpp"
#include "Configuration/IConfigurationBuilder.hpp"
#include "Configuration/IConfigurationProvider.hpp"
#include "Configuration/IConfigurationRoot.hpp"
#include "Configuration/IConfigurationSource.hpp"
#include "Data/DataContainer.hpp"

namespace sd
{
    class ConfigurationManager final : public IConfiguration, public IConfigurationBuilder, public IConfigurationRoot
    {
      private:
        Json::object_t _configuration = {};
        std::vector<IConfigurationSource::Ptr> _sources;
        std::vector<IConfigurationProvider::Ptr> _providers;
        DataContainer _dataContainer;

      public:
        using Ptr = std::unique_ptr<ConfigurationManager>;

        ConfigurationManager() = default;
        ConfigurationManager(ConfigurationManager &&) = default;
        ConfigurationManager(const ConfigurationManager &) = delete;

        ConfigurationManager &operator=(ConfigurationManager &&) = delete;
        ConfigurationManager &operator=(const ConfigurationManager &) = delete;

        void add(IConfigurationSource::Ptr source) { _sources.push_back(std::move(source)); }

        const Json::object_t &root() const { return _configuration; }

        Json::object_t &root() { return _configuration; }

        const Json &at(std::string key) const { return _configuration.at(std::move(key)); }

        Json &at(std::string key) { return _configuration.at(std::move(key)); }

        const Json *find(std::string key) const
        {
            if (auto it = _configuration.find(key); it != _configuration.end())
            {
                return &it->second;
            }
            return nullptr;
        }

        Json *find(std::string_view key)
        {
            if (auto it = _configuration.find(key); it != _configuration.end())
            {
                return &it->second;
            }
            return nullptr;
        }

        Json &operator[](std::string key) { return _configuration[key]; };

        void build()
        {
            _providers.clear();
            for (auto &source : _sources)
            {
                _providers.emplace_back(source->build(*this));
            }
            reload();
        }

        void reload()
        {
            _configuration.clear();
            for (auto &provider : _providers)
            {
                provider->load();
                _configuration.merge(provider->get());
            }
        }

        bool hasAnySources() const { return !_sources.empty(); }

        std::vector<IConfigurationSource::Ptr> &getSources() { return _sources; }

        std::vector<IConfigurationProvider::Ptr> &getProviders() { return _providers; }

        DataContainer &getData() { return _dataContainer; }

        Json::object_t::iterator begin() { return _configuration.begin(); };

        Json::object_t::iterator end() { return _configuration.end(); };

        Json::object_t::const_iterator begin() const { return _configuration.begin(); };

        Json::object_t::const_iterator end() const { return _configuration.end(); };

      private:
    };
} // namespace sd