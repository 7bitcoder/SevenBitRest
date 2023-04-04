#pragma once

#include <cctype>
#include <regex>
#include <string_view>

#include "Common/Utils.hpp"
#include "Router/ISegmentMatcher.hpp"

namespace sd
{
    class StringSegmentMatcher final : public ISegmentMatcher
    {
      private:
        const std::string _segment;

      public:
        StringSegmentMatcher(const std::string &segment) : _segment(segment) {}
        bool match(std::string_view segment) const { return _segment == segment; }
        int precedence() const { return 140; }
    };

    class BoolSegmentMatcher final : public ISegmentMatcher
    {
      public:
        bool match(std::string_view segment) const
        {
            bool res;
            return utils::tryConvert(std::string{segment}, res);
        }
        int precedence() const { return 130; }
    };

    class FloatSegmentMatcher final : public ISegmentMatcher
    {
      public:
        bool match(std::string_view segment) const
        {
            float res;
            return utils::tryConvert(std::string{segment}, res);
        }
        int precedence() const { return 120; }
    };

    class DoubleSegmentMatcher final : public ISegmentMatcher
    {
      public:
        bool match(std::string_view segment) const
        {
            double res;
            return utils::tryConvert(std::string{segment}, res);
        }
        int precedence() const { return 110; }
    };

    class IntSegmentMatcher final : public ISegmentMatcher
    {
      public:
        bool match(std::string_view segment) const
        {
            int res;
            return utils::tryConvert(std::string{segment}, res);
        }
        int precedence() const { return 100; }
    };

    class RequiredSegmentMatcher final : public ISegmentMatcher
    {
      public:
        bool match(std::string_view segment) const { return !segment.empty(); }
        int precedence() const { return 90; }
    };

    class AlphaSegmentMatcher final : public ISegmentMatcher
    {
      public:
        bool match(std::string_view segment) const
        {
            auto res = std::find_if(segment.begin(), segment.end(), [](unsigned char ch) {
                           auto res = std::isalpha(ch);
                           return !res;
                       }) == segment.end();
            return res;
        }
        int precedence() const { return 80; }
    };

    class MinSegmentMatcher final : public ISegmentMatcher
    {
        int _min;

      public:
        MinSegmentMatcher(int min) : _min(min) {}
        bool match(std::string_view segment) const
        {
            int i;
            if (!utils::tryConvert(std::string{segment}, i))
            {
                return false;
            }
            return i >= _min;
        }
        int precedence() const { return 70; }
    };

    class MaxSegmentMatcher final : public ISegmentMatcher
    {
        int _max;

      public:
        MaxSegmentMatcher(int max) : _max(max) {}
        bool match(std::string_view segment) const
        {
            int i;
            if (!utils::tryConvert(std::string{segment}, i))
            {
                return false;
            }
            return i <= _max;
        }
        int precedence() const { return 60; }
    };

    class RangeSegmentMatcher final : public ISegmentMatcher
    {
        int _min;
        int _max;

      public:
        RangeSegmentMatcher(int min, int max) : _min(min), _max(max) {}
        bool match(std::string_view segment) const
        {
            int i;
            if (!utils::tryConvert(std::string{segment}, i))
            {
                return false;
            }
            return i >= _min && i <= _max;
        }
        int precedence() const { return 50; }
    };

    class MinLengthSegmentMatcher final : public ISegmentMatcher
    {
        int _min;

      public:
        MinLengthSegmentMatcher(int min) : _min(min) {}
        bool match(std::string_view segment) const { return segment.size() >= _min; }
        int precedence() const { return 40; }
    };

    class MaxLengthSegmentMatcher final : public ISegmentMatcher
    {
        int _max;

      public:
        MaxLengthSegmentMatcher(int max) : _max(max) {}
        bool match(std::string_view segment) const { return segment.size() <= _max; }
        int precedence() const { return 30; }
    };

    class LengthSegmentMatcher final : public ISegmentMatcher
    {
        int _min;
        int _max;

      public:
        LengthSegmentMatcher(int min, int max) : _min(min), _max(max) {}
        bool match(std::string_view segment) const
        {
            auto size = segment.size();
            return size >= _min && size <= _max;
        }
        int precedence() const { return 20; }
    };

    class RegexSegmentMatcher final : public ISegmentMatcher
    {
        std::regex _regex;

      public:
        RegexSegmentMatcher(std::string regex) : _regex(regex) {}
        bool match(std::string_view segment) const { return std::regex_match(std::string{segment}, _regex); }
        int precedence() const { return 10; }
    };

    class AnySegmentMatcher final : public ISegmentMatcher
    {
      public:
        bool match(std::string_view segment) const { return true; }
        int precedence() const { return 0; }
    };
} // namespace sd