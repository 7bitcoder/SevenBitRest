#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <typeindex>

#include "DI/IServiceHolder.hpp"

namespace sd
{
    struct NullReferenceException : public std::runtime_error
    {
        NullReferenceException() : std::runtime_error{"Null reference"} {}
    };
} // namespace sd