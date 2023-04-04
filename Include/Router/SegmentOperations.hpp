#pragma once

#include <functional>

#include "Router/RouteTemplateSegments.hpp"

template <class T> inline size_t hashType(const T &v) { return typeid(T).hash_code(); }
template <class T> inline void hashCombine(std::size_t &s, const T &v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}
template <> struct std::hash<sd::IntConstrain>
{
    std::size_t operator()(const sd::IntConstrain &segment) const { return hashType(segment); }
};
template <> struct std::hash<sd::BoolConstrain>
{
    std::size_t operator()(const sd::BoolConstrain &segment) const { return hashType(segment); }
};
template <> struct std::hash<sd::FloatConstrain>
{
    std::size_t operator()(const sd::FloatConstrain &segment) const { return hashType(segment); }
};
template <> struct std::hash<sd::DoubleConstrain>
{
    std::size_t operator()(const sd::DoubleConstrain &segment) const { return hashType(segment); }
};
template <> struct std::hash<sd::AnyConstrain>
{
    std::size_t operator()(const sd::AnyConstrain &segment) const { return hashType(segment); }
};
template <> struct std::hash<sd::RequiredConstrain>
{
    std::size_t operator()(const sd::RequiredConstrain &segment) const { return hashType(segment); }
};
template <> struct std::hash<sd::AlphaConstrain>
{
    std::size_t operator()(const sd::AlphaConstrain &segment) const { return hashType(segment); }
};
template <> struct std::hash<sd::MinConstrain>
{
    std::size_t operator()(const sd::MinConstrain &segment) const
    {
        auto result = hashType(segment);
        hashCombine(result, segment.min);
        return result;
    }
};
template <> struct std::hash<sd::MaxConstrain>
{
    std::size_t operator()(const sd::MaxConstrain &segment) const
    {
        auto result = hashType(segment);
        hashCombine(result, segment.max);
        return result;
    }
};
template <> struct std::hash<sd::MinLengthConstrain>
{
    std::size_t operator()(const sd::MinLengthConstrain &segment) const
    {
        auto result = hashType(segment);
        hashCombine(result, segment.min);
        return result;
    }
};
template <> struct std::hash<sd::MaxLengthConstrain>
{
    std::size_t operator()(const sd::MaxLengthConstrain &segment) const
    {
        auto result = hashType(segment);
        hashCombine(result, segment.max);
        return result;
    }
};
template <> struct std::hash<sd::RangeConstrain>
{
    std::size_t operator()(const sd::RangeConstrain &segment) const
    {
        auto result = hashType(segment);
        hashCombine(result, segment.min);
        hashCombine(result, segment.max);
        return result;
    }
};
template <> struct std::hash<sd::LengthConstrain>
{
    std::size_t operator()(const sd::LengthConstrain &segment) const
    {
        auto result = hashType(segment);
        hashCombine(result, segment.min);
        hashCombine(result, segment.max);
        return result;
    }
};
template <> struct std::hash<sd::RegexConstrain>
{
    std::size_t operator()(const sd::RegexConstrain &segment) const
    {
        auto result = hashType(segment);
        hashCombine(result, segment.regex);
        return result;
    }
};
template <> struct std::hash<sd::RouteParam>
{
    std::size_t operator()(const sd::RouteParam &param) const
    {
        return std::hash<sd::RouteParam::Constrain>()(param.constrain);
    }
};
namespace sd
{
    inline bool operator==(const IntConstrain &a, const IntConstrain &b) { return true; }
    inline bool operator==(const BoolConstrain &a, const BoolConstrain &b) { return true; }
    inline bool operator==(const DoubleConstrain &a, const DoubleConstrain &b) { return true; }
    inline bool operator==(const FloatConstrain &a, const FloatConstrain &b) { return true; }
    inline bool operator==(const AnyConstrain &a, const AnyConstrain &b) { return true; }

    inline bool operator==(const RequiredConstrain &a, const RequiredConstrain &b) { return true; }
    inline bool operator==(const AlphaConstrain &a, const AlphaConstrain &b) { return true; }

    inline bool operator==(const MaxConstrain &a, const MaxConstrain &b) { return a.max == b.max; }
    inline bool operator==(const MaxLengthConstrain &a, const MaxLengthConstrain &b) { return a.max == b.max; }
    inline bool operator==(const MinConstrain &a, const MinConstrain &b) { return a.min == b.min; }
    inline bool operator==(const MinLengthConstrain &a, const MinLengthConstrain &b) { return a.min == b.min; }

    inline bool operator==(const RangeConstrain &a, const RangeConstrain &b)
    {
        return a.max == b.max && a.min == b.min;
    }
    inline bool operator==(const LengthConstrain &a, const LengthConstrain &b)
    {
        return a.max == b.max && a.min == b.min;
    }
    inline bool operator==(const RegexConstrain &a, const RegexConstrain &b) { return a.regex == b.regex; }

    inline bool operator==(const RouteParam &a, const RouteParam &b) { return a.constrain == b.constrain; }

} // namespace sd
