#pragma once

#include <memory>

#include "Common/Utils.hpp"
#include "DI/ServiceProvider.hpp"
#include "Engine/BaseParam.hpp"
#include "Engine/IContext.hpp"
#include "Http/IRequest.hpp"

namespace sd
{
    template <class T> class FromServices : BaseParam
    {
      private:
        T &_service;

      public:
        FromServices(IContext &ctx) : BaseParam(ctx), _service(ctx.getRequestServices().getRequiredService<T>()) {}

        T &operator*() { return get(); }

        const T &operator*() const { return get(); }

        T &get() { return _service; }

        const T &get() const { return _service; }
    };

    template <class T> class FromServices<std::unique_ptr<T>> : BaseParam
    {
      private:
        std::unique_ptr<T> _service;

      public:
        FromServices(IContext &ctx) : BaseParam(ctx), _service(ctx.getRequestServices().createService<T>()) {}

        std::unique_ptr<T> &operator*() { return get(); }

        const std::unique_ptr<T> &operator*() const { return get(); }

        std::unique_ptr<T> &get() { return *_service; }

        const std::unique_ptr<T> &get() const { return *_service; }
    };
} // namespace sd