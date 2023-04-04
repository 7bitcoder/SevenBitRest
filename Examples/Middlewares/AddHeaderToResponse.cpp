#include "SevenBitRest.hpp"

using namespace std::string_literals;
using namespace sd;

struct MyMiddleware final : public IMiddleware
{
    void next(IContext &ctx, INextCallback &next)
    {
        next();
        ctx.getResponse().getHeaders().add("My-header", "example value");
    }
};

int main()
{
    auto rest = WebApplicationBuilder{}.build();

    rest.useRouter();

    rest.use<MyMiddleware>();

    rest.useEndpoints();

    rest.mapGet("/", []() { return "Hello, world!"s; });

    rest.run();
}