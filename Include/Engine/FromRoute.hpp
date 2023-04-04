#pragma once

#include "Common/Utils.hpp"
#include "Engine/BaseParam.hpp"
#include "Engine/IContext.hpp"
#include "Http/IRequest.hpp"

namespace sd
{
    template <utils::StringLiteral lit, class T = std::string> class FromRoute : public BaseParam
    {
      private:
        T _value;

      public:
        FromRoute(IContext &ctx) : BaseParam(ctx)
        {
            static_assert(utils::IsSimpleTypeV<T>, "Please use int, bool, float, double or string");
            if constexpr (utils::IsOptionalV<T>)
            {
                if (auto param = ctx.getRoutingData().getRouteParams().get(getName()))
                {
                    _value = utils::convert<typename T::value_type>(std::string{*param});
                }
            }
            else
            {
                auto param = ctx.getRoutingData().getRouteParams().getRequired(getName());
                _value = utils::convert<T>(std::string{param});
            }
        }

        std::string getName() const
        {
            constexpr auto contents = lit.value;
            return {contents};
        }

        T &operator*() { return get(); }

        const T &operator*() const { return get(); }

        T &get() { return _value; }

        const T &get() const { return _value; }
    };
    // aliases
    template <utils::StringLiteral lit> using FromRouteInt = FromRoute<lit, int>;
    template <utils::StringLiteral lit> using FromRouteBool = FromRoute<lit, bool>;
    template <utils::StringLiteral lit> using FromRouteDouble = FromRoute<lit, double>;
    template <utils::StringLiteral lit> using FromRouteFloat = FromRoute<lit, float>;

    template <utils::StringLiteral lit> using FromRouteOpt = FromRoute<lit, std::optional<std::string>>;
    template <utils::StringLiteral lit> using FromRouteIntOpt = FromRoute<lit, std::optional<int>>;
    template <utils::StringLiteral lit> using FromRouteBoolOpt = FromRoute<lit, std::optional<bool>>;
    template <utils::StringLiteral lit> using FromRouteDoubleOpt = FromRoute<lit, std::optional<double>>;
    template <utils::StringLiteral lit> using FromRouteFloatOpt = FromRoute<lit, std::optional<float>>;
} // namespace sd