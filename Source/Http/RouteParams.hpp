#pragma once
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Engine/BoostBeastServer.hpp"
#include "Http/IRouteParams.hpp"
#include "Router/RouteTemplateSegments.hpp"

namespace sd
{
    class RouteParams final : public IRouteParams
    {
      protected:
        std::unordered_map<std::string, std::string> _params;

      public:
        using Ptr = std::unique_ptr<RouteParams>;

        RouteParams() = default;
        RouteParams(RouteParams &&) = default;

        bool has(std::string_view key) const { return _params.contains(std::string{key}); }

        std::string_view getRequired(std::string_view key) const
        {
            if (auto param = get(key))
            {
                return *param;
            }
            throw std::runtime_error("not found param");
        }

        std::optional<std::string_view> get(std::string_view key) const
        {
            auto gg = _params.begin();
            if (auto it = _params.find(std::string{key}); it != _params.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

        size_t size() const { return _params.size(); }

        bool empty() const { return _params.empty(); }

        void forEach(Accesor func) const
        {
            for (auto param : _params)
            {
                if (func(param.first, param.second) == Enumeration::Stop)
                {
                    break;
                }
            }
        }

        void set(std::string_view name, std::string_view value)
        {
            _params.emplace(std::string{name}, std::string{value});
        }

        void add(std::string_view name, std::string_view value) { set(name, value); }

        std::string &getOrAdd(std::string_view name, std::string_view value)
        {
            return _params[std::string{name}] = value;
        }

        void clear() { _params.clear(); }

        bool remove(std::string_view name) { return _params.erase(std::string{name}); }
    };
} // namespace sd