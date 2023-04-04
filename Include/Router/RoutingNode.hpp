#pragma once

#include <unordered_map>

#include "Engine/IEndpoint.hpp"
#include "Http/HttpMethod.hpp"
#include "Router/Exceptions.hpp"
#include "Router/ISegmentMatcher.hpp"

namespace sd
{
    class RoutingNode
    {
      private:
        ISegmentMatcher::Ptr _matcher;
        std::unordered_map<HttpMethod, IEndpoint::Ptr> _endpointsMap;
        std::vector<RoutingNode> _children;

      public:
        RoutingNode(ISegmentMatcher::Ptr matcher) : _matcher(std::move(matcher)) {}

        RoutingNode(RoutingNode &&) = default;
        RoutingNode &operator=(RoutingNode &&) = default;

        void addChild(RoutingNode &&node) { _children.emplace_back(std::move(node)); }

        void addEndpoints(std::vector<IEndpoint::Ptr> endpoints)
        {
            for (auto &endpoint : endpoints)
            {
                addEndpoint(std::move(endpoint));
            }
        }

        void addEndpoint(IEndpoint::Ptr endpoint)
        {
            if (_endpointsMap.contains(endpoint->getHttpMethod()))
            {
                throw EndpointAlreadyRegistered{endpoint->getRouteTemplate(), endpoint->getHttpMethod()};
            }
            _endpointsMap.emplace(endpoint->getHttpMethod(), std::move(endpoint));
        }

        const IEndpoint *getEndpoint(HttpMethod method) const
        {
            if (auto it = _endpointsMap.find(method); it != _endpointsMap.end())
            {
                return it->second.get();
            }
            return nullptr;
        }

        void setChilderen(std::vector<RoutingNode> nodes) { _children = std::move(nodes); }

        bool match(std::string_view segment) const { return _matcher->match(segment); }

        auto begin() { return _children.begin(); }
        auto begin() const { return _children.begin(); }
        auto end() { return _children.end(); }
        auto end() const { return _children.end(); }

        void sortByPrecedence()
        {
            std::sort(begin(), end(), [](auto &a, auto &b) { return a.precedence() > b.precedence(); });
        }

        int precedence() const { return _matcher->precedence(); }
    };
} // namespace sd