#pragma once

#include <functional>

#include "Engine/IContext.hpp"

namespace sd
{
    using Action = std::function<void(IContext &)>;
}