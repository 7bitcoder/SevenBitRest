#pragma once

#include "Http/IParamsView.hpp"
#include <optional>
#include <string>

namespace sd
{
    struct ICookiesView : public IParamsView
    {
        std::optional<std::string_view> operator[](std::string_view key) const { return get(key); }

        virtual ~ICookiesView() = default;
    };
} // namespace sd