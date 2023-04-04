#pragma once
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "Engine/BoostBeastServer.hpp"
#include "Http/IQueryParamsView.hpp"
#include "boost/url/params_view.hpp"

namespace sd
{

    class QueryParamsView final : public IQueryParamsView
    {
      protected:
        const boost::urls::params_view _params;

      public:
        using Ptr = std::unique_ptr<QueryParamsView>;

        QueryParamsView(const boost::urls::params_view params) : _params(params) {}

        bool has(std::string_view key) const { return _params.contains(key); }

        std::string_view getRequired(std::string_view key) const
        {
            if (auto param = get(key))
            {
                return *param;
            }
            throw std::runtime_error("param not found");
        }

        std::optional<std::string_view> get(std::string_view key) const
        {
            if (auto it = _params.find(key); it != _params.end() && (*it)->has_value)
            {
                return (*it)->value;
            }
            return std::nullopt;
        }

        size_t count(std::string_view name) const { return _params.count(name); }

        size_t size() const { return _params.size(); }

        bool empty() const { return _params.empty(); }

        void forEach(Accesor func) const
        {
            for (auto param : _params)
            {
                if (func(param.key, param.value) == Enumeration::Stop)
                {
                    break;
                }
            }
        }

        void forEachOf(std::string_view key, AccesorOf func) const
        {
            for (auto it = _params.find(key), end = _params.find_last(key); it != end; it++)
            {
                const auto &param = *it;
                if (func(param.value) == Enumeration::Stop)
                {
                    break;
                }
            }
        }
    };
} // namespace sd