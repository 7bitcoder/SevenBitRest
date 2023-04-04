#pragma once

#include <memory>

namespace sd
{
    struct IData
    {
        using Ptr = std::unique_ptr<IData>;

        virtual ~IData() = default;
    };
} // namespace sd