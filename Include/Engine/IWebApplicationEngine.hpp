#pragma once

#include <memory>
#include <string_view>
#include <typeindex>

#include "Common/Export.hpp"
#include "Configuration/IConfiguration.hpp"
#include "DI/ServiceProvider.hpp"
#include "Engine/Action.hpp"
#include "Engine/EngineDependencies.hpp"
#include "Engine/IEndpoint.hpp"
#include "Log/ILogger.hpp"
#include "Middlewares/MiddlewareCreator.hpp"
#include "Router/IRouter.hpp"
#include "Services/IEnvironment.hpp"

namespace sd
{
    struct EXPORT IWebApplicationEngine
    {
        using Ptr = std::unique_ptr<IWebApplicationEngine>;

        static Ptr createWebApplicationEngine(EngineDependencies::Ptr dependencies);

        virtual void init() = 0;

        virtual void run(std::optional<std::string> url, int threadsNumber) = 0;

        virtual void stop() = 0;

        virtual IEndpoint *map(HttpMethod method, std::string_view path, Action action) = 0;

        virtual void useAsFirst(IMiddlewareCreator::Ptr middleware) = 0;

        virtual void use(IMiddlewareCreator::Ptr middleware) = 0;

        virtual void useRouter() = 0;

        virtual void useEndpoints() = 0;

        virtual const IConfiguration &getConfiguration() = 0;

        virtual const IEnvironment &getEnvironment() = 0;

        virtual ILogger &getLogger() = 0;

        virtual ServiceProvider &getServiceProvider() = 0;

        virtual ~IWebApplicationEngine() = default;
    };

}; // namespace sd