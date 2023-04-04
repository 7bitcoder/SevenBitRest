#pragma once

#include <iterator>
#include <optional>
#include <string>
#include <string_view>

#include "Engine/BoostBeastServer.hpp"
#include "Http/CommonHeadders.hpp"
#include "Http/ICookiesView.hpp"

namespace sd
{
    class CookiesView final : public ICookiesView
    {
      private:
        NativeParamList _cookies;

      public:
        using Ptr = std::unique_ptr<CookiesView>;

        CookiesView(NativeRequestHeaders &headers) : _cookies(headers[headder::cookie]) {}

        size_t size() const { return std::distance(_cookies.begin(), _cookies.end()); }

        bool empty() const { return _cookies.begin() != _cookies.end(); }

        std::string_view getRequired(std::string_view key) const
        {
            if (auto val = get(key))
            {
                return *val;
            }
            throw std::runtime_error("cookie not found");
        }

        std::optional<std::string_view> get(std::string_view key) const
        {
            for (auto &cookie : _cookies)
            {
                if (cookie.first == key)
                {
                    return cookie.second;
                }
            }
            return std::nullopt;
        }

        std::vector<std::string_view> getKeys() const
        {
            std::vector<std::string_view> result;
            for (auto &cookie : _cookies)
            {
                result.push_back(cookie.first);
            }
            return result;
        }

        bool has(std::string_view key) const
        {
            for (auto &cookie : _cookies)
            {
                if (cookie.first == key)
                {
                    return true;
                }
            }
            return false;
        }

        void forEach(Accesor func) const
        {
            for (auto &cookie : _cookies)
            {
                if (func(cookie.first, cookie.second) == Enumeration::Stop)
                {
                    break;
                }
            }
        }
    };
} // namespace sd