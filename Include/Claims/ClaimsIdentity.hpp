#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "Claims/Claim.hpp"
#include "Claims/ClaimTypes.hpp"
#include "Claims/IIdentity.hpp"

namespace sd
{
    class ClaimsIdentity final : public IIdentity
    {
      private:
        std::vector<Claim> _claims;
        std::optional<std::string> _authenticationType;
        std::string _roleClaimType;
        std::optional<std::string> _name;
        std::string _nameClaimType;
        std::string _label;

      public:
        static inline const std::string DefaultIssuer = "LOCAL AUTHORITY";
        static inline const std::string DefaultNameClaimType = ClaimTypes::Name;
        static inline const std::string DefaultRoleClaimType = ClaimTypes::Role;

        ClaimsIdentity(const ClaimsIdentity &) = default;
        ClaimsIdentity(ClaimsIdentity &&) = default;

        ClaimsIdentity &operator=(const ClaimsIdentity &) = default;
        ClaimsIdentity &operator=(ClaimsIdentity &&) = default;

        ClaimsIdentity() = default;

        ClaimsIdentity(std::vector<Claim> claims) : _claims(std::move(claims)) {}

        ClaimsIdentity(std::vector<Claim> claims, std::string authenticationType)
            : _claims(std::move(claims)), _authenticationType(authenticationType)
        {
        }

        ClaimsIdentity(std::vector<Claim> claims, std::string authenticationType, std::string nameClaimType,
                       std::string roleClaimType)
            : _claims(std::move(claims)), _authenticationType(authenticationType), _roleClaimType(roleClaimType),
              _nameClaimType(nameClaimType)
        {
        }

        ClaimsIdentity(IIdentity &identity)
            : _authenticationType(identity.getAuthenticationType()), _name(identity.getName())
        {
        }

        ClaimsIdentity(IIdentity &identity, std::vector<Claim> claims)
            : _authenticationType(identity.getAuthenticationType()), _name(identity.getName()),
              _claims(std::move(claims))
        {
        }

        ClaimsIdentity(IIdentity &identity, std::vector<Claim> claims, std::string authenticationType,
                       std::string nameClaimType, std::string roleClaimType)
            : _claims(std::move(claims)), _authenticationType(authenticationType), _roleClaimType(roleClaimType),
              _nameClaimType(nameClaimType), _name(identity.getName())
        {
        }

        ClaimsIdentity(std::string authenticationType) : _authenticationType(authenticationType) {}

        ClaimsIdentity(std::string authenticationType, std::string nameClaimType, std::string roleClaimType)
            : _authenticationType(authenticationType), _roleClaimType(roleClaimType), _nameClaimType(nameClaimType)
        {
        }

        std::optional<std::string> getAuthenticationType() const { return _authenticationType; }

        bool isAuthenticated() const { return _authenticationType && !_authenticationType->empty(); }

        std::optional<std::string> getName() const { return _name; }

        std::string getNameClaimType() const { return _nameClaimType; }

        std::string getRoleClaimType() const { return _roleClaimType; }

        std::vector<Claim> getClaims() { return _claims; }

        void addClaim(Claim claim) { _claims.emplace_back(std::move(claim)); }

        void addClaims(std::vector<Claim> claim)
        {
            for (auto &claim : _claims)
            {
                addClaim(std::move(claim));
            }
        }

        std::vector<Claim> findAll(std::function<bool(const Claim &)> lambda) const
        {
            std::vector<Claim> result;
            for (auto &claim : _claims)
            {
                if (lambda(claim))
                {
                    result.push_back(claim);
                }
            }
            return result;
        }

        std::optional<Claim> findFirst(std::function<bool(const Claim &)> lambda) const
        {
            for (auto &claim : _claims)
            {
                if (lambda(claim))
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
            for (auto &claim : _claims)
            {
                if (lambda(claim))
                {
                    return true;
                }
            }
            return false;
        }

        bool hasClaim(std::string_view type, std::string_view value) const
        {
            return hasClaim([&](const Claim &claim) { return claim.getType() == type && claim.getValue() == value; });
        }

        size_t removeClaims(std::string_view type)
        {
            return std::erase_if(_claims, [&](const Claim &claim) { return claim.getType() == type; });
        }
    };
} // namespace sd