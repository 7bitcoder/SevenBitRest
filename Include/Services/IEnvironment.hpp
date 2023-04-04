#pragma once

#include <filesystem>
#include <string>

#include "Common/EnvVariables.hpp"

namespace sd
{
    struct IEnvironment
    {
        virtual const std::string &getEnvironmentName() const = 0;

        virtual const std::string &getApplicationName() const = 0;

        virtual const std::filesystem::path &getContentRootPath() const = 0;

        virtual const std::filesystem::path &getWebRootPath() const = 0;

        virtual bool isProduction() const = 0;

        virtual bool isDevelopment() const = 0;

        virtual bool isEnvironment(std::string_view environmentName) const = 0;

        virtual ~IEnvironment() = default;
    };
} // namespace sd