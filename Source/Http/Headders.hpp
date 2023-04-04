#pragma once

#include <cstddef>
#include <optional>
#include <string_view>
#include <unordered_map>

#include "Engine/BoostBeastServer.hpp"
#include "Http/HeaddersView.hpp"
#include "Http/IHeadders.hpp"

namespace sd
{
    struct StringHash
    {
        using is_transparent = void;
        [[nodiscard]] size_t operator()(const char *txt) const { return std::hash<std::string_view>{}(txt); }
        [[nodiscard]] size_t operator()(std::string_view txt) const { return std::hash<std::string_view>{}(txt); }
        [[nodiscard]] size_t operator()(const std::string &txt) const { return std::hash<std::string>{}(txt); }
    };

    class Headders final : public IHeadders
    {
      private:
        NativeResponseHeaders &_native;
        std::unordered_map<std::string, std::vector<std::string>, StringHash> _headers;

      public:
        using Ptr = std::unique_ptr<Headders>;

        Headders(NativeResponseHeaders &native) : _native(native) {}

        std::string_view getRequired(std::string_view name) const { return _headers.at(std::string{name}).front(); }

        std::optional<std::string_view> get(std::string_view name) const
        {
            if (auto it = _headers.find(std::string{name}); it != _headers.end() && !it->second.empty())
            {
                return it->second.front();
            }
            return std::nullopt;
        }

        size_t size() const { return _headers.size(); }

        size_t count(std::string_view name) const
        {
            if (auto it = _headers.find(std::string{name}); it != _headers.end())
            {
                return it->second.size();
            }
            return 0;
        }

        bool empty() const { return _headers.empty(); }

        bool has(std::string_view name) const { return _headers.contains(std::string{name}); }

        void set(std::string_view name, std::string_view value)
        {
            if (auto it = _headers.find(std::string{name}); it != _headers.end() && !it->second.empty())
            {
                it->second.front() = value;
            }
        }

        void add(std::string_view name, std::string_view value)
        {
            return _headers[std::string{name}].push_back(std::string{value});
        }

        std::string &getOrAdd(std::string_view key, std::string_view value)
        {
            if (auto it = _headers.find(std::string{key}); it != _headers.end() && !it->second.empty())
            {
                return it->second.front() = value;
            }
            return _headers[std::string{key}].emplace_back(std::string{value});
        }

        size_t size(std::string_view key) const
        {
            if (auto it = _headers.find(std::string{key}); it != _headers.end())
            {
                return it->second.size();
            };
            return 0;
        }

        bool remove(std::string_view name)
        {
            if (auto it = _headers.find(std::string{name}); it != _headers.end())
            {
                auto rm = it->second.erase(it->second.begin()) != it->second.end();
                if (it->second.empty())
                {
                    _headers.erase(it);
                }
                return rm;
            }
            return false;
        }

        size_t removeAll(std::string_view name) { return _headers.erase(std::string{name}); }

        void clear() { _headers.clear(); }

        // std::uint64_t getContentLength() const { auto len = _native.at() }

        // void setContentLength(std::uint64_t length) = 0;

        // bool getKeepAlive() const = 0;

        // void setKeepAlive(bool keepAlive) = 0;

        // Connection getConnection() const = 0;

        // void setConnection(Connection connection) = 0;

        // std::uint16_t getHttpMajor() const = 0;

        // void setHttpMajor(std::uint16_t v) = 0;

        // std::uint16_t getHttpMinor() const = 0;

        // void setHttpMinor(std::uint16_t v) = 0;

        void forEach(Accesor accesor) const
        {
            for (auto &field : _headers)
            {
                for (auto &header : field.second)
                {
                    if (accesor(field.first, header) == Enumeration::Stop)
                    {
                        goto stop;
                    }
                }
            }
        stop:
            return;
        }

        void forEachOf(std::string_view name, AccesorOf accesor) const
        {
            if (auto it = _headers.find(std::string{name}); it != _headers.end())
            {
                for (auto &value : it->second)
                {
                    if (accesor(value) == Enumeration::Stop)
                    {
                        return;
                    }
                }
            }
        }

        void build()
        {
            forEach([this](std::string_view key, std::string_view value) {
                _native.insert(key, value);
                return Enumeration::Continue;
            });
        }
        ~Headders() = default;
    };
} // namespace sd