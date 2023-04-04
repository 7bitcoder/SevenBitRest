#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "Http/IParams.hpp"
#include "Http/IRouteParamsView.hpp"

namespace sd
{
    struct IRouteParams : public IRouteParamsView, public IParams
    {
        using Ptr = std::unique_ptr<IRouteParamsView>;

        std::string &operator[](std::string_view key) { return getOrAdd(key); }

        virtual ~IRouteParams() = default;
    };
} // namespace sd