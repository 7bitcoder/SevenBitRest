#pragma once

#include "Common/Utils.hpp"
#include "Engine/IEndpoint.hpp"
#include "Router/IRouter.hpp"
#include "Router/RoutingTree.hpp"

namespace sd
{
    class Router final : public IRouter
    {
      private:
        RoutingTree _tree;

      public:
        IEndpoint *addEndpoint(IEndpoint::Ptr endpoint) { return _tree.add(std::move(endpoint)); };

        void compile() { _tree.compile(); };

        const IEndpoint *match(HttpMethod method, std::string_view path) const
        {
            auto splitted = utils::split(path);
            if (auto endpoint = _tree.match(method, splitted))
            {
                return endpoint;
            }
            return nullptr;
        }
    };
} // namespace sd