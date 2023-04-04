#pragma once

#include <memory>
#include <string_view>

#include "Data/IData.hpp"

namespace sd
{
    struct IDataContainer
    {
        using Ptr = std::unique_ptr<IDataContainer>;

        virtual IData *get(std::string_view key) const = 0;

        virtual void add(std::string_view key, IData::Ptr data) = 0;

        virtual bool remove(std::string_view key) = 0;

        virtual bool exists(std::string_view key) const = 0;

        virtual ~IDataContainer() = default;
    };
} // namespace sd