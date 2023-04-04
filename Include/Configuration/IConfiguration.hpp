#pragma once

#include <string>
#include <tao/json/forward.hpp>

#include "Common/Json.hpp"

namespace sd
{
    struct IConfiguration
    {
        virtual const Json::object_t &root() const = 0;

        virtual const Json &at(std::string key) const = 0;

        virtual const Json *find(std::string key) const = 0;

        virtual const Json &operator[](std::string key) const { return at(key); };

        virtual Json::object_t::const_iterator begin() const = 0;

        virtual Json::object_t::const_iterator end() const = 0;

        virtual ~IConfiguration() = default;
    };
} // namespace sd