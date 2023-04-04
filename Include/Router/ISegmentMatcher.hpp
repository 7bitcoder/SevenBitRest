#pragma once

#include <memory>
#include <string_view>

namespace sd
{
    struct ISegmentMatcher
    {
        using Ptr = std::unique_ptr<ISegmentMatcher>;

        virtual bool match(std::string_view segment) const = 0;

        virtual int precedence() const = 0;

        virtual ~ISegmentMatcher() = default;
    };
} // namespace sd