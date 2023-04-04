#pragma once

#include <vector>

#include "Claims/ClaimTypes.hpp"
#include "Claims/ClaimsIdentity.hpp"
#include "Claims/IIdentity.hpp"
#include "Claims/IPrincipal.hpp"

namespace sd
{
    class ClaimsPrincipal final : public IPrincipal
    {
      private:
        std::vector<ClaimsIdentity> _identities;

      public:
        using Ptr = std::unique_ptr<ClaimsPrincipal>;

        ClaimsPrincipal() = default;

        ClaimsPrincipal(ClaimsIdentity identity) { _identities.push_back(identity); }

        IIdentity &getIdentity() { return _identities.front(); };

        std::vector<ClaimsIdentity> &getIdentities() { return _identities; };

        void addIdentities(std::vector<ClaimsIdentity> idetities)
        {
            for (auto &identity : idetities)
            {
                addIdentity(std::move(identity));
            }
        }

        void addIdentity(ClaimsIdentity identity) { _identities.emplace_back(std::move(identity)); }

        std::vector<Claim> findAll(std::function<bool(const Claim &)> lambda) const
        {
            std::vector<Claim> result;
            for (auto &identity : _identities)
            {
                for (auto &claim : identity.findAll(lambda))
                {
                    result.push_back(claim);
                }
            }
            return result;
        }

        std::vector<Claim> findAll(std::string_view type) const
        {
            std::vector<Claim> result;
            for (auto &identity : _identities)
            {
                if (auto claim = identity.findFirst(type))
                {
                    result.push_back(*claim);
                }
            }
            return result;
        }

        std::optional<Claim> findFirst(std::function<bool(const Claim &)> lambda) const
        {
            for (auto &identity : _identities)
            {
                if (auto claim = identity.findFirst(lambda))
                {
                    return claim;
                }
            }
            return std::nullopt;
        }

        std::optional<Claim> findFirst(std::string_view type) const
        {
            return findFirst([&](const Claim &claim) { return claim.getType() == type; });
        }

        bool hasClaim(std::function<bool(const Claim &)> lambda) const
        {
            for (auto &identity : _identities)
            {
                if (identity.hasClaim(lambda))
                {
                    return true;
                }
            }
            return false;
        }

        template <class Lambda> bool hasClaim(std::string_view type, std::string_view value) const
        {
            return hasClaim([&](Claim &claim) { return claim.getType() == type && claim.getValue() == value; });
        }

        bool isInRole(std::string_view role) const
        {
            if (auto claim = findFirst(ClaimTypes::Role))
            {
                return claim->getValue() == role;
            }
            return false;
        };
    };
} // namespace sd