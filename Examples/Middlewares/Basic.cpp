#include "SevenBitRest.hpp"

using namespace std::string_literals;
using namespace sd;

struct MyMiddleware final : public IMiddleware
{
    void next(IContext &ctx, INextCallback &next)
    {
        // pre actions
        next();
        // post actions
    }
};

int main()
{
    auto rest = WebApplicationBuilder{}.build();

    rest.use<MyMiddleware>();

    rest.mapGet("/", []() { return "Hello, world!"s; });

    rest.run();
}