#pragma once

#include <filesystem>
#include <memory>

#include "Services/IEnvironment.hpp"

namespace sd
{
    class Environment final : public IEnvironment
    {
      private:
        std::string _envitonmentName = EnvironmentNames::Production;
        std::string _applicationName = "7BitRest Service";
        std::filesystem::path _contentRootPath = std::filesystem::current_path();
        std::filesystem::path _webRootPath = _contentRootPath / "wwwroot";

      public:
        using Ptr = std::unique_ptr<Environment>;

        void setEnvironmentName(std::string envitonmentName) { _envitonmentName = envitonmentName; }

        const std::string &getEnvironmentName() const { return _envitonmentName; }

        void setApplicationName(std::string applicationName) { _applicationName = applicationName; }

        const std::string &getApplicationName() const { return _applicationName; }

        void setContentRootPath(std::filesystem::path contentRootPath) { _contentRootPath = contentRootPath; }

        const std::filesystem::path &getContentRootPath() const { return _contentRootPath; }

        void setWebRootPath(std::filesystem::path webRootPath) { _webRootPath = webRootPath; }

        virtual const std::filesystem::path &getWebRootPath() const { return _webRootPath; }

        bool isProduction() const { return isEnvironment(EnvironmentNames::Production); }

        bool isDevelopment() const { return isEnvironment(EnvironmentNames::Development); }

        bool isEnvironment(std::string_view environmentName) const { return _envitonmentName == environmentName; }
    };
} // namespace sd