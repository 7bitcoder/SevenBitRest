#pragma once

#include "Http/IParamsView.hpp"
#include <optional>
#include <string>
namespace sd
{

    struct IParams : public virtual IParamsView
    {
        virtual void set(std::string_view key, std::string_view value) = 0;

        virtual std::string &getOrAdd(std::string_view key, std::string_view value = "") = 0;

        virtual void add(std::string_view key, std::string_view value) = 0;

        virtual bool remove(std::string_view key) = 0;

        virtual void clear() = 0;

        std::string &operator[](std::string_view key) { return getOrAdd(key); }

        virtual ~IParams() = default;
    };
} // namespace sd