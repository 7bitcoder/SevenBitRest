#pragma once

#include "Services/IEnvironment.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace sd
{
    struct WebApplicationOptions
    {
        int argc = 0;

        char **argv = nullptr;

        std::string environmentPrefix = EnvironmentVariables::Prefix;

        std::optional<std::string> applicationName;

        std::optional<std::filesystem::path> contentRootPath;

        std::optional<std::string> environmentName;

        std::optional<std::filesystem::path> webRootPath;
    };
} // namespace sd