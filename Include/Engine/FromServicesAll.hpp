#pragma once

#include <memory>

#include "Common/Utils.hpp"
#include "DI/ServiceProvider.hpp"
#include "Engine/BaseParam.hpp"
#include "Engine/IContext.hpp"
#include "Http/IRequest.hpp"

namespace sd
{
    template <class T> class FromServicesAll : BaseParam
    {
      private:
        std::vector<T *> _services;

      public:
        using Type = T;

        FromServicesAll(IContext &ctx) : BaseParam(ctx), _services(ctx.getRequestServices().getServices<T>()) {}

        std::vector<T *> &operator*() { return get(); }

        const std::vector<T *> &operator*() const { return get(); }

        std::vector<T *> &get() { return _services; }

        const std::vector<T *> &get() const { return _services; }
    };

    template <class T> class FromServicesAll<std::vector<std::unique_ptr<T>>> : BaseParam
    {
      private:
        std::vector<std::unique_ptr<T>> _services;

      public:
        using Type = T;

        FromServicesAll(IContext &ctx) : BaseParam(ctx), _services(ctx.getRequestServices().createServices<T>()) {}

        std::vector<std::unique_ptr<T>> &operator*() { return get(); }

        const std::vector<std::unique_ptr<T>> &operator*() const { return get(); }

        std::vector<std::unique_ptr<T>> &get() { return _services; }

        const std::vector<std::unique_ptr<T>> &get() const { return _services; }
    };
} // namespace sd