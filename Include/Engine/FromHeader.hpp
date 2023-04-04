#pragma once

#include "Common/Utils.hpp"
#include "Engine/BaseParam.hpp"
#include "Engine/IContext.hpp"
#include "Http/IRequest.hpp"

namespace sd
{
    template <utils::StringLiteral lit, class T = std::string> class FromHeader : public BaseParam
    {
      private:
        T _value;

      public:
        FromHeader(IContext &ctx) : BaseParam(ctx)
        {
            static_assert(utils::IsSimpleTypeV<T>, "Please use int, bool, float, double or string");
            if constexpr (utils::IsOptionalV<T>)
            {
                if (auto param = ctx.getRequest().getHeaders().get(getName()))
                {
                    _value = utils::convert<typename T::value_type>(std::string{*param});
                }
            }
            else
            {
                auto param = ctx.getRequest().getHeaders().getRequired(getName());
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
    template <utils::StringLiteral lit> using FromHeaderInt = FromHeader<lit, int>;
    template <utils::StringLiteral lit> using FromHeaderBool = FromHeader<lit, bool>;
    template <utils::StringLiteral lit> using FromHeaderDouble = FromHeader<lit, double>;
    template <utils::StringLiteral lit> using FromHeaderFloat = FromHeader<lit, float>;

    template <utils::StringLiteral lit> using FromHeaderOpt = FromHeader<lit, std::optional<std::string>>;
    template <utils::StringLiteral lit> using FromHeaderIntOpt = FromHeader<lit, std::optional<int>>;
    template <utils::StringLiteral lit> using FromHeaderBoolOpt = FromHeader<lit, std::optional<bool>>;
    template <utils::StringLiteral lit> using FromHeaderDoubleOpt = FromHeader<lit, std::optional<double>>;
    template <utils::StringLiteral lit> using FromHeaderFloatOpt = FromHeader<lit, std::optional<float>>;
} // namespace sd