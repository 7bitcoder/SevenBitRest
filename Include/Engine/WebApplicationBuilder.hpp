#pragma once

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdio.h>
#include <tao/json/forward.hpp>
#include <tao/json/from_file.hpp>
#include <tao/json/type.hpp>

#include "Common/CliVariables.hpp"
#include "Common/EnvVariables.hpp"
#include "Common/Json.hpp"
#include "Common/ServerSettings.hpp"
#include "Common/Utils.hpp"
#include "Configuration/AppSettingsConfigurationSource.hpp"
#include "Configuration/CliConfigurationSource.hpp"
#include "Configuration/ConfigurationManager.hpp"
#include "Configuration/EnvConfigurationSource.hpp"
#include "Configuration/IConfiguration.hpp"
#include "Configuration/JsonConfigurationSource.hpp"
#include "DI/ServiceProvider.hpp"
#include "Engine/EnvironmentBuilder.hpp"
#include "Engine/WebApplication.hpp"
#include "Engine/WebApplicationOptions.hpp"
#include "Log/ConsoleLogProvider.hpp"
#include "Log/Logger.hpp"
#include "Log/LoggingBuilder.hpp"
#include "Router/Router.hpp"
#include "Services/Environment.hpp"
#include "Services/IEnvironment.hpp"
#include "Services/ServerSettingsProvider.hpp"

namespace sd
{
    class WebApplicationBuilder
    {
      private:
        Environment::Ptr _environment;
        ConfigurationManager::Ptr _configuration;
        LoggingBuilder::Ptr _loggingBuilder;
        ServiceCollection::Ptr _serviceCollection;
        IRouter::Ptr _router;
        ServerSettings _serverSettings;

      public:
        WebApplicationBuilder(int argc, char **argv) : WebApplicationBuilder({.argc = argc, .argv = argv}) {}
        WebApplicationBuilder(WebApplicationOptions options = {})
        {
            _environment = EnvironmentBuilder{}.build(options);
            _configuration = std::make_unique<ConfigurationManager>();
            _loggingBuilder = std::make_unique<LoggingBuilder>();
            _serviceCollection = std::make_unique<ServiceCollection>();

            init(options);
        }

        WebApplicationBuilder(const WebApplicationBuilder &) = delete;
        WebApplicationBuilder(WebApplicationBuilder &&) = delete;
        WebApplicationBuilder &operator=(const WebApplicationBuilder &) = delete;
        WebApplicationBuilder &operator=(WebApplicationBuilder &&) = delete;

        WebApplication build()
        {
            registerDefaultServices();

            if (!_router)
            {
                _router = std::make_unique<Router>();
            }
            auto dependencies = std::make_unique<EngineDependencies>(std::move(_serviceCollection), std::move(_router));
            return {std::move(dependencies)};
        }

        ServiceCollection &getServices() { return *_serviceCollection; }

        LoggingBuilder &getLogging() { return *_loggingBuilder; }

        ConfigurationManager &getConfiguration() { return *_configuration; }

        Environment &getEnvironment() { return *_environment; }

        ServerSettings &getServerSettings() { return _serverSettings; }

        WebApplicationBuilder &useRouter(IRouter::Ptr router)
        {
            _router = std::move(router);
            return *this;
        }

        WebApplicationBuilder &configureAppConfiguration(std::function<void(ConfigurationManager &)> action)
        {
            action(getConfiguration());
            return *this;
        }

        WebApplicationBuilder &configureServices(std::function<void(ServiceCollection &)> action)
        {
            action(getServices());
            return *this;
        }

        WebApplicationBuilder &configureLogging(std::function<void(LoggingBuilder &)> action)
        {
            action(getLogging());
            return *this;
        }

        WebApplicationBuilder &configureServerSettings(std::function<void(ServerSettings &)> action)
        {
            action(getServerSettings());
            return *this;
        }

        WebApplicationBuilder &useSetting(std::string key, std::string value)
        {
            Json::object_t json = {{key, value}};
            getConfiguration().add(std::make_unique<JsonConfigurationSource>(std::move(json)));
            return *this;
        }

        WebApplicationBuilder &useSettings(Json::object_t json)
        {
            getConfiguration().add(std::make_unique<JsonConfigurationSource>(json));
            return *this;
        }

        WebApplicationBuilder &useContentRoot(std::filesystem::path path)
        {
            getEnvironment().setContentRootPath(path);
            return *this;
        }

        WebApplicationBuilder &useEnvironment(std::string environment)
        {
            getEnvironment().setEnvironmentName(environment);
            return *this;
        }

        WebApplicationBuilder &useWebRoot(std::filesystem::path path)
        {
            getEnvironment().setWebRootPath(path);
            return *this;
        }

        WebApplicationBuilder &useApplicationName(std::string name)
        {
            getEnvironment().setApplicationName(name);
            return *this;
        }

        WebApplicationBuilder &useUrls(std::vector<std::string> urls)
        {
            getServerSettings().urls = urls;
            return *this;
        }

        WebApplicationBuilder &useUrl(std::string url) { return useUrls({url}); }

      private:
        void init(const WebApplicationOptions &options)
        {
            _configuration->add(
                std::make_unique<AppSettingsConfigurationSource>(getEnvironment().getEnvironmentName()));
            _configuration->add(std::make_unique<EnvConfigurationSource>(options.environmentPrefix));
            _configuration->add(std::make_unique<CliConfigurationSource>(options.argc, options.argv));

            _loggingBuilder->add<ConsoleLogProvider>();
        }

        void registerDefaultServices()
        {
            _serviceCollection->addSingleton<IEnvironment, Environment>(
                [env = std::move(_environment)](ServiceProvider &) {
                    return std::move(const_cast<Environment::Ptr &>(env));
                });

            _serviceCollection->addSingleton<IConfiguration, ConfigurationManager>(
                [conf = std::move(_configuration)](ServiceProvider &) {
                    return std::move(const_cast<ConfigurationManager::Ptr &>(conf));
                });

            _serviceCollection->addSingleton<IServerSettingsProvider, ServerSettingsProvider>(
                [settings = std::move(_serverSettings)](ServiceProvider &provider) {
                    auto &config = provider.getRequiredService<IConfiguration>();
                    return std::make_unique<ServerSettingsProvider>(&config, settings);
                });

            _serviceCollection->merge(std::move(_loggingBuilder->getServices()));
            _serviceCollection->addSingleton<ILogger, Logger>();
        }
    };
}; // namespace sd