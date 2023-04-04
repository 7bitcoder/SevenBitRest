#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <variant>

#include "Common/Utils.hpp"
#include "Engine/IEndpoint.hpp"
#include "Http/HttpMethod.hpp"
#include "Router/ISegmentMatcher.hpp"
#include "Router/RouteTemplateSegments.hpp"
#include "Router/RoutingNode.hpp"
#include "Router/SegmentMatchers.hpp"
#include "Router/SegmentOperations.hpp"

namespace sd
{
    class RoutingTree
    {
      private:
        using Endpoints = std::vector<IEndpoint::Ptr>;
        using RouteSegments = std::vector<std::string_view>;

        RoutingNode _root{std::make_unique<StringSegmentMatcher>("")};
        Endpoints _endpoints;

      public:
        IEndpoint *add(IEndpoint::Ptr endpoint) { return _endpoints.emplace_back(std::move(endpoint)).get(); }

        const IEndpoint *match(HttpMethod method, const RouteSegments &routeSegments) const
        {
            if (routeSegments.empty())
            {
                return nullptr;
            }
            return getMatching(_root, method, routeSegments, 0);
        }

        void compile()
        {
            _root.setChilderen(createRoutingNodes(std::move(_endpoints), 0));
            sortNodesByPrecedence(_root);
        }

      private:
        void sortNodesByPrecedence(RoutingNode &node)
        {
            node.sortByPrecedence();

            for (auto &child : node)
            {
                sortNodesByPrecedence(child);
            }
        }

        const IEndpoint *getMatching(const RoutingNode &node, HttpMethod method, const RouteSegments &routeSegments,
                                     int depth) const
        {
            if (!node.match(routeSegments[depth]))
            {
                return nullptr;
            }
            if (routeSegments.size() - 1 == depth)
            {
                return node.getEndpoint(method);
            }
            for (auto &child : node)
            {
                if (auto matchingNode = getMatching(child, method, routeSegments, depth + 1))
                {
                    return matchingNode;
                }
            }
            return nullptr;
        }

        std::vector<RoutingNode> createRoutingNodes(Endpoints allEndpoints, int depth) const
        {
            auto lookup = getEndpointsByRouteTemplateSegment(std::move(allEndpoints), depth);
            std::vector<RoutingNode> result;

            for (auto &data : lookup)
            {
                auto &[routeTemplateSegment, endpoints] = data;
                RoutingNode node{createMatcher(routeTemplateSegment)};

                node.addEndpoints(moveOutMatchingDepthEndpoints(endpoints, depth));
                node.setChilderen(createRoutingNodes(std::move(endpoints), depth + 1));

                result.push_back(std::move(node));
            }
            return result;
        }

        std::unordered_map<RouteTemplateSegment, Endpoints> getEndpointsByRouteTemplateSegment(Endpoints endpoints,
                                                                                               int depth) const
        {
            std::unordered_map<RouteTemplateSegment, Endpoints> result;
            for (auto &endpoint : endpoints)
            {
                auto &routeTemplateSegment = endpoint->getRouteTemplateSegments().at(depth);
                result[routeTemplateSegment].emplace_back(std::move(endpoint));
            }
            return result;
        }

        Endpoints moveOutMatchingDepthEndpoints(Endpoints &endpoints, int depth) const
        {
            Endpoints result;
            for (auto &endpoint : endpoints)
            {
                auto &routeTemplateSegments = endpoint->getRouteTemplateSegments();
                if (routeTemplateSegments.size() - 1 == depth)
                {
                    result.push_back(std::move(endpoint));
                }
            }
            std::erase_if(endpoints, [](auto &endpoint) { return !endpoint; });
            return result;
        }

        ISegmentMatcher::Ptr createMatcher(const RouteTemplateSegment &routeTemplateSegment) const
        {
            if (const auto segment = std::get_if<std::string>(&routeTemplateSegment))
            {
                return std::make_unique<StringSegmentMatcher>(*segment);
            }
            return createMatcherFromConstrain(std::get<RouteParam>(routeTemplateSegment).constrain);
        }

        ISegmentMatcher::Ptr createMatcherFromConstrain(const RouteParam::Constrain &constrain) const
        {
            using Ptr = ISegmentMatcher::Ptr;
            return std::visit(
                utils::overloaded{/******** auto format force :) ********/
                                  [](const RequiredConstrain &c) { return Ptr{new RequiredSegmentMatcher{}}; },
                                  [](const IntConstrain &c) { return Ptr{new IntSegmentMatcher{}}; },
                                  [](const BoolConstrain &c) { return Ptr{new BoolSegmentMatcher{}}; },
                                  [](const FloatConstrain &c) { return Ptr{new FloatSegmentMatcher{}}; },
                                  [](const DoubleConstrain &c) { return Ptr{new DoubleSegmentMatcher{}}; },
                                  [](const AlphaConstrain &c) { return Ptr{new AlphaSegmentMatcher{}}; },
                                  [](const MaxConstrain &c) { return Ptr{new MaxSegmentMatcher{c.max}}; },
                                  [](const MinConstrain &c) { return Ptr{new MinSegmentMatcher{c.min}}; },
                                  [](const MaxLengthConstrain &c) { return Ptr{new MaxLengthSegmentMatcher{c.max}}; },
                                  [](const MinLengthConstrain &c) { return Ptr{new MinLengthSegmentMatcher{c.min}}; },
                                  [](const LengthConstrain &c) {
                                      return Ptr{new LengthSegmentMatcher{c.min, c.max}};
                                  },
                                  [](const RangeConstrain &c) {
                                      return Ptr{new RangeSegmentMatcher{c.min, c.max}};
                                  },
                                  [](const RegexConstrain &c) { return Ptr{new RegexSegmentMatcher{c.regex}}; },
                                  [](const AnyConstrain &c) { return Ptr{new AnySegmentMatcher{}}; },
                                  [](const auto &c) { return Ptr{new AnySegmentMatcher{}}; }},
                constrain);
        }
    };
} // namespace sd