#pragma once

#include "Configuration/IConfiguration.hpp"
#include "DI/ServiceProvider.hpp"
#include "Log/ILogger.hpp"
#include "Router/IRouter.hpp"
#include "Services/IEnvironment.hpp"
#include "Services/IServerSettingsProvider.hpp"

namespace sd
{
    class EngineDependencies
    {
      private:
        ServiceCollection::Ptr _serviceCollection;
        ServiceContainer _singletonsContainer;
        ServiceProvider _serviceProvider;

        IRouter::Ptr _router;

        IConfiguration &_configuration;
        IEnvironment &_environment;
        ILogger &_logger;

        IServerSettingsProvider &_settingProvider;

      public:
        using Ptr = std::unique_ptr<EngineDependencies>;

        EngineDependencies(ServiceCollection::Ptr serviceCollection, IRouter::Ptr router)
            : _router(moveAndCheck(std::move(router))), /****/
              _serviceCollection(moveAndCheck(std::move(serviceCollection))),
              _serviceProvider(*_serviceCollection, _singletonsContainer),
              _configuration(_serviceProvider.getRequiredService<IConfiguration>()),
              _environment(_serviceProvider.getRequiredService<IEnvironment>()),
              _logger(_serviceProvider.getRequiredService<ILogger>()),
              _settingProvider(_serviceProvider.getRequiredService<IServerSettingsProvider>())
        {
        }

        IRouter &getRouter() const { return *_router; }

        ServiceProvider &getServiceProvider() { return _serviceProvider; }

        IConfiguration &getConfiguration() { return _configuration; }

        IEnvironment &getEnvironment() { return _environment; }

        ILogger &getLogger() { return _logger; }

        IServerSettingsProvider &getSettingsProvider() { return _settingProvider; }

      private:
        ServiceCollection::Ptr moveAndCheck(ServiceCollection::Ptr serviceCollection)
        {
            if (!serviceCollection)
            {
                throw std::runtime_error("router is null");
            }
            return serviceCollection;
        }

        IRouter::Ptr moveAndCheck(IRouter::Ptr router)
        {
            if (!router)
            {
                throw std::runtime_error("router is null");
            }
            return router;
        }
    };
} // namespace sd