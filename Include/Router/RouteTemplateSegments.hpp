#pragma once

#include <cstddef>
#include <regex>
#include <stdexcept>
#include <variant>
#include <vector>

#include "Common/Utils.hpp"
#include "Router/Constrains.hpp"
#include "Router/Exceptions.hpp"

namespace sd
{
    struct RouteParam
    {
        using Constrain =
            std::variant<IntConstrain, BoolConstrain, FloatConstrain, DoubleConstrain, AnyConstrain, RequiredConstrain,
                         AlphaConstrain, MinConstrain, MaxConstrain, MinLengthConstrain, MaxLengthConstrain,
                         RangeConstrain, LengthConstrain, RegexConstrain>;
        std::string name;
        Constrain constrain;
    };

    using RouteTemplateSegment = std::variant<std::string, RouteParam>;

    class RouteTemplateSegments
    {
      private:
        std::vector<RouteTemplateSegment> _segments;

      public:
        RouteTemplateSegments(std::string_view routeTemplate)
        {
            _segments = createRouteTemplateSegments(routeTemplate);
        }

        auto begin() const { return _segments.begin(); }
        auto end() const { return _segments.end(); }

        size_t size() const { return _segments.size(); }

        const RouteTemplateSegment &at(size_t index) const { return _segments.at(index); }

      private:
        std::vector<RouteTemplateSegment> createRouteTemplateSegments(std::string_view path)
        {
            if (!path.starts_with("/"))
            {
                throw BadPath{path};
            }
            std::vector<RouteTemplateSegment> segments;

            auto segmentsStr = utils::split(path);
            if (segmentsStr.empty())
            {
                throw BadPath{path};
            }
            segmentsStr.erase(segmentsStr.begin());
            for (auto segmentStr : segmentsStr)
            {
                segments.push_back(createRouteTemplateSegment(std::string{segmentStr}));
            }
            return segments;
        }

        RouteTemplateSegment createRouteTemplateSegment(const std::string &segmentStr)
        {
            std::smatch matches;
            if (matchRouteParam(segmentStr, matches))
            {
                auto name = matches[1].str();
                return RouteParam{.name = name, .constrain = createConstrain(segmentStr)};
            }
            return std::string{segmentStr};
        }

        RouteParam::Constrain createConstrain(const std::string &segmentStr)
        {
            std::smatch matches;
            if (matchConstrain(segmentStr, "int|bool|string|double|float|any|required|alpha", matches))
            {
                auto name = matches[3].str();
                return createBasicConstrain(name);
            }
            if (matchConstrain(segmentStr, "(minlength|maxlength|min|max)\\((\\d+)\\)", matches))
            {
                auto function = matches[4].str();
                int arg = std::stoi(matches[5].str());

                return createFunctionConstrain(function, arg);
            }
            if (matchConstrain(segmentStr, "(length|range)\\((\\d+),(\\d+)\\)", matches))
            {
                auto function = matches[4].str();
                int arg1 = std::stoi(matches[5].str());
                int arg2 = std::stoi(matches[6].str());

                return createFunctionConstrain(function, arg1, arg2);
            }
            if (matchConstrain(segmentStr, "regex\\((.*)\\)", matches))
            {
                auto regex = matches[4].str();

                return RegexConstrain{regex};
            }
            return AnyConstrain{};
        }

        bool matchConstrain(const std::string &string, std::string pattern, std::smatch &result)
        {
            return std::regex_match(string, result, std::regex{"^\\{(\\w+)(\\:(" + pattern + "))\\}"});
        }

        bool matchRouteParam(const std::string &string, std::smatch &result)
        {
            return std::regex_match(string, result, std::regex{"^\\{(\\w+).*\\}"});
        }

        RouteParam::Constrain createBasicConstrain(std::string_view name)
        {
            if (name == "int")
            {
                return IntConstrain{};
            }
            if (name == "bool")
            {
                return BoolConstrain{};
            }
            if (name == "double")
            {
                return DoubleConstrain{};
            }
            if (name == "float")
            {
                return FloatConstrain{};
            }
            if (name == "alpha")
            {
                return AlphaConstrain{};
            }
            if (name == "required")
            {
                return RequiredConstrain{};
            }
            return AnyConstrain{};
        }

        RouteParam::Constrain createFunctionConstrain(std::string_view functionName, int arg)
        {
            if (functionName == "max")
            {
                return MaxConstrain{arg};
            }
            if (functionName == "min")
            {
                return MinConstrain{arg};
            }
            if (arg < 1)
            {
                throw std::runtime_error("Function argument should be a positive integer");
            }
            if (functionName == "minlength")
            {
                return MinLengthConstrain{arg};
            }
            if (functionName == "maxlength")
            {
                return MaxLengthConstrain{arg};
            }

            throw std::runtime_error("wrong function name");
        }

        RouteParam::Constrain createFunctionConstrain(std::string_view functionName, int arg1, int arg2)
        {
            if (functionName == "range")
            {
                return RangeConstrain{arg1, arg2};
            }
            if (arg1 < 1 || arg2 < arg1)
            {
                throw std::runtime_error("First function argument should be a positive integer and second argument "
                                         "should be greater than first");
            }
            if (functionName == "length")
            {
                return LengthConstrain{arg1, arg2};
            }
            throw std::runtime_error("wrong function name");
        }
    };
} // namespace sd