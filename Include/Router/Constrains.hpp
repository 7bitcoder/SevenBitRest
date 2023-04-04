#pragma once

#include <string>

namespace sd
{
    struct IntConstrain
    {
    };

    struct BoolConstrain
    {
    };

    struct FloatConstrain
    {
    };

    struct DoubleConstrain
    {
    };

    struct AnyConstrain
    {
    };

    struct RequiredConstrain
    {
    };

    struct AlphaConstrain
    {
    };

    struct MaxLengthConstrain
    {
        int max = 0;
    };

    struct MinLengthConstrain
    {
        int min = 0;
    };

    struct MaxConstrain
    {
        int max = 0;
    };

    struct MinConstrain
    {
        int min = 0;
    };

    struct RangeConstrain
    {
        int min = 0;
        int max = 0;
    };

    struct LengthConstrain
    {
        int min = 0;
        int max = 0;
    };

    struct RegexConstrain
    {
        std::string regex;
    };
} // namespace sd