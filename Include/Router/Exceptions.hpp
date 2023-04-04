#pragma once

#include <exception>
#include <stdexcept>
#include <string>

#include "Http/HttpMethod.hpp"

namespace sd
{
    struct EndpointAlreadyRegistered : public std::runtime_error
    {
        EndpointAlreadyRegistered(std::string_view templateRoute, HttpMethod method)
            : std::runtime_error{std::string{"Endpoint with template route: '"} + std::string{templateRoute} +
                                 "' and method '" + std::to_string(method) + "' is already registered."}
        {
        }
    };

    struct BadPath : public std::runtime_error
    {
        BadPath(std::string_view path)
            : std::runtime_error{std::string{"Path :'"} + std::string{path} + "' doesnt start with '/'"}
        {
        }
    };
} // namespace sd