#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "DI/IServiceHolder.hpp"
#include "DI/ServiceList.hpp"

namespace sd
{
    class ServiceContainer
    {
      private:
        std::unordered_map<TypeId, ServiceList> _serviceListMap;

      public:
        ServiceList *addAndGetList(IServiceHolder::Ptr holder)
        {
            if (!holder)
            {
                throw ServiceHolderInvalidException{};
            }
            auto interfaceTypeId = holder->getServiceInterfaceTypeId();
            auto &serviceList = _serviceListMap[interfaceTypeId];
            serviceList.add(std::move(holder));
            return &serviceList;
        }

        ServiceList *getList(TypeId interfaceType)
        {
            auto it = _serviceListMap.find(interfaceType);
            return it != _serviceListMap.end() ? &it->second : nullptr;
        }

        const ServiceList *getList(TypeId interfaceType) const
        {
            auto it = _serviceListMap.find(interfaceType);
            return it != _serviceListMap.end() ? &it->second : nullptr;
        }

        bool contains(TypeId interfaceType) const { return getList(interfaceType); }
    };

} // namespace sd