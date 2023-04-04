#pragma once

#include <memory>
#include <string>

#include "Engine/BoostBeastServer.hpp"
#include "Http/CommonHeadders.hpp"
#include "Http/CookiesView.hpp"
#include "Http/HeaddersView.hpp"
#include "Http/IRequest.hpp"
#include "Http/QueryParamsView.hpp"
#include "boost/url/scheme.hpp"
#include "boost/url/src.hpp"

namespace sd
{
    class Request final : public IRequest
    {
      private:
        NativeRequest &_native;
        const boost::url_view _url;

        mutable HeaddersView::Ptr _headers;
        mutable QueryParamsView::Ptr _query;
        mutable CookiesView::Ptr _cookies;

      public:
        Request(NativeRequest &native) : _native(native), _url(Request::parseUrl(native.target())) {}

        using Ptr = std::unique_ptr<Request>;

        std::string getBody() const { return _native.body(); }

        uint64_t getContentLength() const { return 1; }

        std::string_view getContentType() const { return getHeaders().getRequired(headder::content_type); }

        const ICookiesView &getCookies() const
        {
            if (!_cookies)
            {
                _cookies = std::make_unique<CookiesView>(_native.base());
            }
            return *_cookies;
        }

        bool hasFormContentType() const { return getContentType() == "multipart/form-data"; }

        const IHeaddersView &getHeaders() const
        {
            if (!_headers)
            {
                _headers = std::make_unique<HeaddersView>(_native.base());
            }
            return *_headers;
        }

        std::string_view getHost() const { return getHeaders().getRequired(headder::host); }

        bool isHttps() const { return _url.scheme_id() == boost::urls::scheme::https; }

        HttpMethod getMethod() const
        {
            using method = boost::beast::http::verb;
            switch (_native.method())
            {
            case method::get:
                return HttpMethod::Get;
            case method::post:
                return HttpMethod::Post;
            case method::put:
                return HttpMethod::Put;
            case method::delete_:
                return HttpMethod::Delete;
            case method::patch:
                return HttpMethod::Patch;
            case method::head:
                return HttpMethod::Head;
            default:
                return HttpMethod::Unsupported;
            };
        }

        std::string getPath() const { return _url.path(); }

        unsigned int getHttpVersion() const { return _native.version(); }

        const IQueryParamsView &getQuery() const
        {
            if (!_query)
            {
                _query = std::make_unique<QueryParamsView>(_url.params());
            }
            return *_query;
        }

        std::string_view getQueryString() const { return _url.encoded_query(); }

        std::string_view getContentEncoding() const { return getHeaders().getRequired(headder::content_encoding); }

        std::string_view getScheme() const { return _url.scheme(); }

        ~Request() = default;

      private:
        static boost::url_view parseUrl(std::string_view url)
        {
            auto result = boost::urls::parse_origin_form(url);
            if (result.has_error())
            {
                throw std::runtime_error("Invalid url");
            }
            return *result;
        }
    };

} // namespace sd