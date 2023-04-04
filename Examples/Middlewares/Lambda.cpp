#include "SevenBitRest.hpp"

using namespace std::string_literals;
using namespace sd;

int main()
{
    auto rest = WebApplicationBuilder{}.build();

    rest.use([](IContext &ctx, INextCallback &next) {
        // pre actions
        next();
        // post actions
    });

    rest.mapGet("/", []() { return "Hello, world!"s; });

    rest.run();
}