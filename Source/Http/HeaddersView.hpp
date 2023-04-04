#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

#include "Engine/BoostBeastServer.hpp"
#include "Http/IHeaddersView.hpp"

namespace sd
{
    class HeaddersView : public virtual IHeaddersView
    {
      protected:
        NativeRequestHeaders &_native;

      public:
        using Ptr = std::unique_ptr<HeaddersView>;

        HeaddersView(NativeRequestHeaders &native) : _native(native) {}

        std::string_view getRequired(std::string_view name) const { return _native.at(name); }

        std::optional<std::string_view> get(std::string_view name) const { return _native[name]; }

        bool has(std::string_view name) const { return _native.find(name) != _native.end(); }

        bool empty() const { return _native.begin() != _native.end(); }

        size_t size() const { return std::distance(_native.begin(), _native.end()); }

        size_t count(std::string_view name) const { return _native.count(name); }

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
            for (auto &field : _native)
            {
                if (accesor(field.name_string(), field.value()) == Enumeration::Stop)
                {
                    return;
                }
            }
        }

        void forEachOf(std::string_view name, AccesorOf accesor) const
        {
            auto [begin, end] = _native.equal_range(name);
            for (auto it = begin; it != end; it++)
            {
                auto &field = *it;
                if (accesor(field.value()) == Enumeration::Stop)
                {
                    return;
                }
            }
        }

        ~HeaddersView() = default;
    };
} // namespace sd