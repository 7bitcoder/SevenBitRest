#pragma once

#include "Common/Json.hpp"
#include "Common/Utils.hpp"
#include "Engine/BaseParam.hpp"
#include "Engine/IContext.hpp"
#include "Http/IRequest.hpp"
#include "Http/Results.hpp"
#include <tao/json/forward.hpp>
#include <tao/json/from_string.hpp>

namespace sd
{
    template <class T> class FromBody : public BaseParam
    {
      private:
        T _value;

      public:
        FromBody(IContext &ctx) : BaseParam(ctx), _value(getJson(ctx).template as<T>()) {}

        T &operator*() { return get(); }

        const T &operator*() const { return get(); }

        T &get() { return _value; }

        const T &get() const { return _value; }

      private:
        Json getJson(IContext &ctx) { return tao::json::basic_from_string<JsonTraits>(ctx.getRequest().getBody()); }
    };

    template <> class FromBody<Json> : public BaseParam
    {
      private:
        Json _value;

      public:
        FromBody(IContext &ctx) : BaseParam(ctx), _value(getJson(ctx)) {}

        Json &operator*() { return get(); }

        const Json &operator*() const { return get(); }

        Json &get() { return _value; }

        const Json &get() const { return _value; }

      private:
        Json getJson(IContext &ctx) { return tao::json::basic_from_string<JsonTraits>(ctx.getRequest().getBody()); }
    };

    template <> class FromBody<std::string> : public BaseParam
    {
      private:
        std::string _value;

      public:
        FromBody(IContext &ctx) : BaseParam(ctx), _value(ctx.getRequest().getBody()) {}

        std::string &operator*() { return get(); }

        const std::string &operator*() const { return get(); }

        std::string &get() { return _value; }

        const std::string &get() const { return _value; }
    };
} // namespace sd