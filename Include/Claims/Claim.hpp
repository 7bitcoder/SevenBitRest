#pragma once

#include <memory>
#include <string>

namespace sd
{
    class ClaimsIdentity;
    class Claim
    {
      private:
        std::string _type;
        std::string _value;
        std::string _valueType;
        std::string _issuer;
        std::string _originalIssuer;
        std::shared_ptr<ClaimsIdentity> _claimsIdentity;

      public:
        Claim(const Claim &) = default;
        Claim(Claim &&) = default;

        Claim &operator=(const Claim &) = default;
        Claim &operator=(Claim &&) = default;

        Claim(Claim claim, std::shared_ptr<ClaimsIdentity> identity) : Claim(claim)
        {
            _claimsIdentity = std::move(identity);
        }

        Claim(std::string type, std::string value) : _type(type), _value(value) {}

        Claim(std::string type, std::string value, std::string valueType)
            : _type(type), _value(value), _valueType(valueType)
        {
        }

        Claim(std::string type, std::string value, std::string valueType, std::string issuer)
            : _type(type), _value(value), _valueType(valueType), _issuer(issuer)
        {
        }

        Claim(std::string type, std::string value, std::string valueType, std::string issuer,
              std::string originalIssuer)
            : _type(type), _value(value), _valueType(valueType), _issuer(issuer), _originalIssuer(originalIssuer)
        {
        }

        Claim(std::string type, std::string value, std::string valueType, std::string issuer,
              std::string originalIssuer, std::shared_ptr<ClaimsIdentity> identity)
            : _type(type), _value(value), _valueType(valueType), _issuer(issuer), _originalIssuer(originalIssuer),
              _claimsIdentity(std::move(identity))
        {
        }

        const std::string &getType() const { return _type; }

        const std::string &getValue() const { return _value; }

        const std::string &getValueType() const { return _valueType; }

        const std::string &getIssuer() const { return _issuer; }

        const std::string &getOriginalIssuer() const { return _originalIssuer; }

        const ClaimsIdentity &getClaimsIdentity() const { return *_claimsIdentity; }

        std::string toString() const { return _type + ": " + _value; }
    };
} // namespace sd