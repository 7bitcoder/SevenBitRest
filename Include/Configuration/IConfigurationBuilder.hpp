#pragma once

#include <memory>
#include <unordered_map>

#include "Common/Json.hpp"
#include "Configuration/IConfigurationSource.hpp"
#include "Data/IData.hpp"
#include "Data/IDataContainer.hpp"

namespace sd
{
    struct IConfigurationBuilder
    {
        using Ptr = std::unique_ptr<IConfigurationBuilder>;

        virtual void build() = 0;

        virtual void add(IConfigurationSource::Ptr) = 0;

        virtual std::vector<IConfigurationSource::Ptr> &getSources() = 0;

        virtual IDataContainer &getData() = 0;

        virtual ~IConfigurationBuilder() = default;
    };
} // namespace sd