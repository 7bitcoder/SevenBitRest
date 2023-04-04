#include "SevenBitRest.hpp"

using namespace std::string_literals;
using namespace sd;

int main()
{
    auto rest = WebApplicationBuilder{}.build();

    rest.mapGet("/", []() { return "Hello, world!"s; });

    rest.run();
}