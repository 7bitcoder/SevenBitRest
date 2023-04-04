#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace sd
{
    class CliVariables
    {
      private:
        std::unordered_map<std::string, std::string> _variables;

      public:
        CliVariables(std::unordered_map<std::string, std::string> variables) : _variables(variables) {}
        CliVariables(CliVariables &&) = default;
        CliVariables(const CliVariables &) = default;

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