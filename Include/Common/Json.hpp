#pragma once

#include <tao/json.hpp>

namespace sd
{
    template <class T> struct JsonTraits : public tao::json::traits<T>
    {
    };

    using Json = tao::json::basic_value<JsonTraits>;
} // namespace sd