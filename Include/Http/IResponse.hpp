#pragma once

#include <memory>

#include "Http/IHeadders.hpp"

namespace sd
{
    struct IResponse
    {
        using Ptr = std::unique_ptr<IResponse>;

        virtual void setBody(const std::string value) = 0;

        virtual void setStatusCode(int statusCode) = 0;

        virtual IHeadders &getHeaders() = 0;

        virtual ~IResponse() = default;
    };

} // namespace sd