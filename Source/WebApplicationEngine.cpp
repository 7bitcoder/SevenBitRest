#include "Engine/WebApplicationEngine.hpp"

namespace sd
{
    IWebApplicationEngine::Ptr IWebApplicationEngine::createWebApplicationEngine(EngineDependencies::Ptr dependencies)
    {
        return IWebApplicationEngine::Ptr{new WebApplicationEngine{std::move(dependencies)}};
    }
} // namespace sd