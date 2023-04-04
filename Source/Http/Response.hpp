#pragma once

#include <memory>

#include "Engine/BoostBeastServer.hpp"
#include "Http/Headders.hpp"
#include "Http/IResponse.hpp"
#include "Http/Request.hpp"

namespace sd
{
    class Response : public IResponse
    {
      private:
        Request &_request;
        NativeResponse _native;

        Headders::Ptr _headers;

      public:
        using Ptr = std::unique_ptr<Response>;

        Response(Request &request)
            : _request(request), _native(boost::beast::http::status::ok, _request.getHttpVersion())
        {
        }

        void setStatusCode(int statusCode) { _native.result(statusCode); }

        IHeadders &getHeaders()
        {
            if (!_headers)
            {
                _headers = std::make_unique<Headders>(_native.base());
            }
            return *_headers;
        }

        void setBody(const std::string value) { _native.body() = std::move(value); }

        NativeResponse getNative()
        {
            _native.prepare_payload();
            return _native;
        }

        ~Response() = default;
    };

} // namespace sd