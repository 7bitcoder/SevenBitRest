#pragma once

#include <memory>

#include "Common/Json.hpp"
#include "Http/IResponse.hpp"
#include "Http/IResult.hpp"

namespace sd
{

    class JsonResult : public IResult
    {
      private:
        sd::Json _value;
        int _statusCode;

      public:
        JsonResult(sd::Json value, int statusCode = 200) : _value(std::move(value)), _statusCode(statusCode) {}

        void execute(IResponse &response)
        {
            response.setStatusCode(_statusCode);
            response.getHeaders().add("Content-Type", "application/json; charset=utf-8");
            response.setBody(tao::json::to_string(_value));
        }
    };

    class OkResult final : public JsonResult
    {

      public:
        OkResult(sd::Json value) : JsonResult(std::move(value)) {}
    };

    class TextResult final : public IResult
    {
      private:
        std::string _value;
        std::string _contentType;

      public:
        TextResult(std::string value, std::string contentType)
            : _value(std::move(value)), _contentType(std::move(contentType))
        {
        }
        void execute(IResponse &response)
        {
            response.setStatusCode(200);
            response.getHeaders().add("Content-Type", _contentType);
            response.setBody(_value);
        }
    };

    class BytesResult final : public IResult
    {
        void execute(IResponse &response) {}
    };

    class StreamResult final : public IResult
    {
        void execute(IResponse &response) {}
    };

    class FileResult final : public IResult
    {
        void execute(IResponse &response) {}
    };
    class NotFoundResult final : public IResult
    {
        void execute(IResponse &response) {}
    };
    class NoContentResult final : public IResult
    {
        void execute(IResponse &response) {}
    };
    class UnprocessableEntityResult final : public IResult
    {
        void execute(IResponse &response) {}
    };
    class BadRequestResult final : public IResult
    {
        void execute(IResponse &response) {}
    };
    class ConflictResult final : public IResult
    {
        void execute(IResponse &response) {}
    };
    class ProblemResult final : public IResult
    {
        void execute(IResponse &response) {}
    };
    class ValidationProblemResult final : public IResult
    {
        void execute(IResponse &response) {}
    };

    namespace Results
    {
        template <class T> IResult::Ptr Ok(T value) { return std::make_unique<OkResult>(value); }
        template <class T> IResult::Ptr Json(T value, int statusCode)
        {
            return std::make_unique<JsonResult>(value, statusCode);
        }
        inline IResult::Ptr Text(std::string value, std::string contentType)
        {
            return std::make_unique<TextResult>(std::move(value), std::move(contentType));
        }
    } // namespace Results

} // namespace sd