#pragma once

#include <optional>
#include <string>
namespace sd
{

    struct IParamsView
    {
        enum Enumeration
        {
            Continue,
            Stop
        };

        using Accesor = std::function<Enumeration(std::string_view, std::string_view)>;

        virtual std::optional<std::string_view> get(std::string_view key) const = 0;

        virtual std::string_view getRequired(std::string_view key) const = 0;

        virtual bool has(std::string_view key) const = 0;

        virtual size_t size() const = 0;

        virtual bool empty() const = 0;

        virtual void forEach(Accesor func) const = 0;

        std::optional<std::string_view> operator[](std::string_view key) const { return get(key); }

        virtual ~IParamsView() = default;
    };
} // namespace sd