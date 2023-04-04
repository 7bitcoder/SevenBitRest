#pragma once

#include "Common/Utils.hpp"
#include "Engine/BaseParam.hpp"
#include "Engine/IContext.hpp"
#include "Http/IQueryParamsView.hpp"
#include "Http/IRequest.hpp"

namespace sd
{
    template <utils::StringLiteral lit, class T = std::string> class FromQuery : public BaseParam
    {
      private:
        T _value;

      public:
        FromQuery(IContext &ctx) : BaseParam(ctx)
        {
            static_assert(utils::IsSimpleTypeV<T>, "Please use int, bool, float, double or string");
            if constexpr (utils::IsOptionalV<T>)
            {
                if (auto param = ctx.getRequest().getQuery().get(getName()))
                {
                    _value = utils::convert<typename T::value_type>(std::string{*param});
                }
            }
            else
            {
                auto param = ctx.getRequest().getQuery().getRequired(getName());
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
    template <utils::StringLiteral lit> using FromQueryInt = FromQuery<lit, int>;
    template <utils::StringLiteral lit> using FromQueryBool = FromQuery<lit, bool>;
    template <utils::StringLiteral lit> using FromQueryDouble = FromQuery<lit, double>;
    template <utils::StringLiteral lit> using FromQueryFloat = FromQuery<lit, float>;

    template <utils::StringLiteral lit> using FromQueryOpt = FromQuery<lit, std::optional<std::string>>;
    template <utils::StringLiteral lit> using FromQueryIntOpt = FromQuery<lit, std::optional<int>>;
    template <utils::StringLiteral lit> using FromQueryBoolOpt = FromQuery<lit, std::optional<bool>>;
    template <utils::StringLiteral lit> using FromQueryDoubleOpt = FromQuery<lit, std::optional<double>>;
    template <utils::StringLiteral lit> using FromQueryFloatOpt = FromQuery<lit, std::optional<float>>;
} // namespace sd