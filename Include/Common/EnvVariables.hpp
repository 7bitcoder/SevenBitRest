#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace sd
{
    namespace EnvironmentNames
    {
        const std::string Production = "production";
        const std::string Development = "development";
    } // namespace EnvironmentNames

    namespace EnvironmentVariables
    {
        const std::string Prefix = "7BITREST_";
        const std::string EnvironmentName = "ENVIRONMENT";
        const std::string ApplicationName = "APPLICATION_NAME";
        const std::string ContentRootPath = "CONTENT_ROOT_PATH";
        const std::string WebRootPath = "WEB_ROOT_PATH";
    } // namespace EnvironmentVariables

    class EnvVariables
    {
      private:
        std::unordered_map<std::string, std::string> _variables;

      public:
        EnvVariables(std::unordered_map<std::string, std::string> variables) : _variables(variables) {}
        EnvVariables(EnvVariables &&) = default;
        EnvVariables(const EnvVariables &) = default;

        bool contains(std::string key) { return _variables.contains(key); }

        std::optional<std::string> operator[](std::string key)
        {
            if (auto it = _variables.find(key); it != _variables.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

        auto begin() const { return _variables.begin(); }

        auto end() const { return _variables.end(); }
    };
} // namespace sd