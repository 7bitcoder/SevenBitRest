#pragma once

#include <filesystem>
#include <memory>

#include "Common/Utils.hpp"
#include "Engine/WebApplicationOptions.hpp"
#include "Services/Environment.hpp"
#include "Services/IEnvironment.hpp"

namespace sd
{
    class EnvironmentBuilder
    {
      public:
        Environment::Ptr build(const WebApplicationOptions &options)
        {
            auto env = std::make_unique<Environment>();
            setEnvironmentWithEnvVariables(*env, options.environmentPrefix);
            setEnvironmentWithCliVariables(*env, options.argc, options.argv);
            setEnvironmentWithOptions(*env, options);
            return env;
        }

      private:
        void setEnvironmentWithEnvVariables(Environment &env, const std::string &prefix)
        {
            auto getEnvVariable = [&](const std::string &key) { return std::getenv((prefix + key).c_str()); };
            if (auto var = getEnvVariable(EnvironmentVariables::EnvironmentName))
            {
                env.setEnvironmentName(var);
            }
            if (auto var = getEnvVariable(EnvironmentVariables::ApplicationName))
            {
                env.setApplicationName(var);
            }
            if (auto var = getEnvVariable(EnvironmentVariables::ContentRootPath))
            {
                env.setContentRootPath(var);
            }
            if (auto var = getEnvVariable(EnvironmentVariables::WebRootPath))
            {
                env.setWebRootPath(var);
            }
        }

        void setEnvironmentWithCliVariables(Environment &env, int argc, char **argv)
        {
            auto cliVariables = utils::getCliVariables(argc, argv);
            if (auto var = cliVariables[EnvironmentVariables::EnvironmentName])
            {
                env.setEnvironmentName(*var);
            }
            if (auto var = cliVariables[EnvironmentVariables::ApplicationName])
            {
                env.setApplicationName(*var);
            }
            if (auto var = cliVariables[EnvironmentVariables::ContentRootPath])
            {
                env.setContentRootPath(*var);
            }
            if (auto var = cliVariables[EnvironmentVariables::WebRootPath])
            {
                env.setWebRootPath(*var);
            }
        }

        void setEnvironmentWithOptions(Environment &env, const WebApplicationOptions &options)
        {
            if (options.environmentName)
            {
                env.setEnvironmentName(*options.environmentName);
            }
            if (options.applicationName)
            {
                env.setApplicationName(*options.applicationName);
            }
            if (options.contentRootPath)
            {
                env.setContentRootPath(*options.contentRootPath);
            }
            if (options.webRootPath)
            {
                env.setWebRootPath(*options.webRootPath);
            }
        }
    };
} // namespace sd