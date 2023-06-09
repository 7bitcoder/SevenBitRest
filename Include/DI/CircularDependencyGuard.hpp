#pragma once

#include <exception>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include "Common/Utils.hpp"
#include "DI/Exceptions.hpp"
#include "DI/IServiceHolder.hpp"

namespace sd
{
    class CircularDependencyGuard
    {
      private:
        std::unordered_set<TypeId> _typeIdsUnderConstruction;

      public:
        class SopedGuard
        {
          public:
            std::unordered_set<TypeId> &_typeIdsUnderConstruction;
            TypeId _typeIdUnderConstruction;

            SopedGuard(TypeId typeIdUnderConstruction, std::unordered_set<TypeId> &typeIdsUnderConstruction)
                : _typeIdUnderConstruction(typeIdUnderConstruction), _typeIdsUnderConstruction(typeIdsUnderConstruction)
            {
                if (_typeIdsUnderConstruction.contains(_typeIdUnderConstruction))
                {
                    throw CircularDependencyException{_typeIdUnderConstruction};
                }
                _typeIdsUnderConstruction.insert(_typeIdUnderConstruction);
            }

            ~SopedGuard() { _typeIdsUnderConstruction.erase(_typeIdUnderConstruction); }
        };

        SopedGuard spawnGuard(TypeId typeIdUnderConstruction)
        {
            return SopedGuard{typeIdUnderConstruction, _typeIdsUnderConstruction};
        }
    };
} // namespace sd