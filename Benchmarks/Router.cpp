#include <benchmark/benchmark.h>

#include "Engine/Endpoint.hpp"
#include "Engine/IContext.hpp"
#include "Http/HttpMethod.hpp"
#include "Router/Router.hpp"

static void RouterBenchmark(benchmark::State &state)
{
    sd::Router router;
    router.addEndpoint(
        std::make_unique<sd::Endpoint>(sd::HttpMethod::Get, "/api/users/{id:int}", [](sd::IContext &) {}));
    router.addEndpoint(std::make_unique<sd::Endpoint>(sd::HttpMethod::Get, "/api/users/{id:int}/settings/email",
                                                      [](sd::IContext &) {}));
    router.addEndpoint(
        std::make_unique<sd::Endpoint>(sd::HttpMethod::Post, "/api/users/{id:int}/settings", [](sd::IContext &) {}));
    router.addEndpoint(std::make_unique<sd::Endpoint>(sd::HttpMethod::Get, "/api/users", [](sd::IContext &) {}));

    router.compile();

    for (auto _ : state)
        router.match(sd::HttpMethod::Get, "/api/users/12");
}

BENCHMARK(RouterBenchmark)->Complexity()->MeasureProcessCPUTime();

BENCHMARK_MAIN();