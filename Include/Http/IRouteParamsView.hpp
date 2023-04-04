#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "Http/IParamsView.hpp"

namespace sd
{
    struct IRouteParamsView : public IParamsView
    {
        using Ptr = std::unique_ptr<IRouteParamsView>;

        std::optional<std::string_view> operator[](std::string_view key) const { return get(key); }

        virtual ~IRouteParamsView() = default;
    };
} // namespace sd