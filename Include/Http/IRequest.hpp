#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <string_view>

#include "Http/HttpMethod.hpp"
#include "Http/ICookiesView.hpp"
#include "Http/IHeaddersView.hpp"
#include "Http/IQueryParamsView.hpp"
#include "Http/IRouteParamsView.hpp"

namespace sd
{
    struct IRequest
    {
        using Ptr = std::unique_ptr<IRequest>;

        virtual std::string getBody() const = 0;

        //  io BodyReader

        virtual uint64_t getContentLength() const = 0;

        virtual std::string_view getContentType() const = 0;

        virtual const ICookiesView &getCookies() const = 0;

        // Form

        virtual bool hasFormContentType() const = 0;

        virtual const IHeaddersView &getHeaders() const = 0;

        virtual std::string_view getHost() const = 0;

        virtual bool isHttps() const = 0;

        virtual HttpMethod getMethod() const = 0;

        virtual std::string getPath() const = 0;

        virtual unsigned int getHttpVersion() const = 0;

        virtual const IQueryParamsView &getQuery() const = 0;

        virtual std::string_view getQueryString() const = 0;

        virtual std::string_view getContentEncoding() const = 0;

        // virtual const IRouteParamsView &getRouteValues() const = 0;

        virtual std::string_view getScheme() const = 0;

        virtual ~IRequest() = default;
    };

} // namespace sd