#pragma once

#include <string_view>

#include "DI/ServiceProvider.hpp"
#include "Log/ILogProvider.hpp"
#include "Log/LogMarkers.hpp"

namespace sd
{
    class LoggingBuilder final
    {
      private:
        ServiceCollection _logProviders;

      public:
        using Ptr = std::unique_ptr<LoggingBuilder>;

        ServiceCollection &getServices() { return _logProviders; }

        void clearProviders() { getServices().removeAll<ILogProvider>(); }

        template <class T> void add()
        {
            static_assert(std::is_base_of_v<ILogProvider, T>, "Type T must inherit from ILogProvider");
            getServices().addSingleton<ILogProvider, T>();
        }

        template <class T> bool contains() { return getServices().contains<ILogProvider, T>(); }

        template <class T> bool remove() { return getServices().remove<ILogProvider, T>(); }
    };
} // namespace sd