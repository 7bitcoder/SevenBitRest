#pragma once

#include <memory>

#include "Http/IResponse.hpp"

namespace sd
{
    struct IResult
    {
        using Ptr = std::unique_ptr<IResult>;

        virtual void execute(IResponse &response) = 0;

        virtual ~IResult() = default;
    };
} // namespace sd