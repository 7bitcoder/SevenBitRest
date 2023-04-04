#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "Http/IHeaddersView.hpp"
#include "Http/IParams.hpp"

namespace sd
{
    struct IHeadders : public IHeaddersView, public IParams
    {
        using Ptr = std::unique_ptr<IHeadders>;

        virtual size_t removeAll(std::string_view name) = 0;

        // virtual std::uint64_t getContentLength() const = 0;

        // virtual void setContentLength(std::uint64_t length) = 0;

        // virtual bool getKeepAlive() const = 0;

        // virtual void setKeepAlive(bool keepAlive) = 0;

        // virtual Connection getConnection() const = 0;

        // virtual void setConnection(Connection connection) = 0;

        // virtual std::uint16_t getHttpMajor() const = 0;

        // virtual void setHttpMajor(std::uint16_t v) = 0;

        // virtual std::uint16_t getHttpMinor() const = 0;

        // virtual void setHttpMinor(std::uint16_t v) = 0;

        std::string &operator[](std::string_view name) { return getOrAdd(name); }

        virtual ~IHeadders() = default;
    };

} // namespace sd