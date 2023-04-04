#pragma once

#include "Services/IContextAccessor.hpp"

namespace sd
{
    class ContextAccessor final : public IContextAccessor
    {
      private:
        IContext *_context;

      public:
        IContext &get() { return *_context; }

        void set(IContext *context) final
        {
            if (!context)
            {
                throw std::runtime_error("context is null");
            }
            _context = context;
        }
    };
} // namespace sd