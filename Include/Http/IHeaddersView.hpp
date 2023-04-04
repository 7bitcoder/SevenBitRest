#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "Http/IParamsView.hpp"

namespace sd
{
    struct IHeaddersView : public virtual IParamsView
    {
        using AccesorOf = std::function<Enumeration(std::string_view)>;

        virtual size_t count(std::string_view key) const = 0;

        virtual void forEachOf(std::string_view key, AccesorOf func) const = 0;

        std::optional<std::string_view> operator[](std::string_view key) const { return get(key); }

        virtual ~IHeaddersView() = default;
    };

} // namespace sd