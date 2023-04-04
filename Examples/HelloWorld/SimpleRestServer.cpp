#include <string>

#include "SevenBitRest.hpp"

struct Interface
{
    virtual std::string gimeRes() const { return "hello"; }
};

struct ImplementationA : public Interface
{
    std::string gimeRes() const { return "hello from A"; }
};

struct ImplementationB : public Interface
{
    std::string gimeRes() const { return "hello from B"; }
};

struct C
{
    const Interface *_a;
    sd::ServiceProvider *_provider;
    sd::IContext *_ctx;
    C(const Interface *a, sd::ServiceProvider *provider, sd::IContextAccessor *ctx) : _a(a), _provider(provider)
    {
        _ctx = &ctx->get();
    }
};

struct ScopedInterface
{
};
struct D
{
    std::vector<Interface *> _vec;
    std::vector<std::unique_ptr<ScopedInterface>> _transients;

    D(std::vector<Interface *> vec, std::vector<std::unique_ptr<ScopedInterface>> transients)
        : _vec(vec), _transients(std::move(transients))
    {
    }
};

struct ScopedImplementationA : public ScopedInterface
{
};
struct ScopedImplementationB : public ScopedInterface
{
};

struct singeleton
{
};

struct result
{
    std::string name;
    int id;
};

template <> struct sd::JsonTraits<result>
{
    template <template <typename...> class Traits>
    static void assign(tao::json::basic_value<Traits> &v, const result &t)
    {
        v = {{"name", t.name}, {"id", t.id}};
    }

    template <template <typename...> class Traits> static void to(const tao::json::basic_value<Traits> &v, result &d)
    {
        const auto &object = v.get_object();
        d.name = v.at("name").template as<std::string>();
        d.id = v.at("id").template as<int>();
    }

    template <template <typename...> class Traits> static result as(const tao::json::basic_value<Traits> &v)
    {
        result result;
        const auto &object = v.get_object();
        result.name = v.at("name").template as<std::string>();
        result.id = v.at("id").template as<int>();
        return result;
    }
};

int main(int argc, char *argv[])
{
    using namespace std::string_literals;
    using namespace sd;

    auto rest = WebApplicationBuilder{argc, argv}
                    .useUrls({"https://localhost:9091", "http://localhost:9090", "http://localhost:8080"})
                    .configureServices([](ServiceCollection &services) {
                        services.addSingleton<singeleton>();
                        services.addScoped<Interface, ImplementationA>();
                        services.addScoped<Interface, ImplementationB>();
                        services.addScoped<C>();
                        services.addScoped<D>();
                        services.addTransient<ScopedInterface, ScopedImplementationA>();
                        services.addTransient<ScopedInterface, ScopedImplementationB>();

                        services.addContextAccessor();
                    })
                    .build();

    rest.mapGet("/{id:int}/bob",
                [](std::vector<Interface *> vec, std::vector<std::unique_ptr<ScopedInterface>> transients,
                   std::unique_ptr<ScopedInterface> scoped, FromQueryIntOpt<"token"> token, FromRouteIntOpt<"id"> id,
                   FromServices<D> service, C *c, IContext &ctx, const IContext *ctx1, const C *const aFromDI,
                   IEnvironment *env, IConfiguration *conf, ServiceProvider &provider) {
                    std::vector<Interface *> allInterfaces = provider.getServices<Interface>();
                    return result{.name = "bob", .id = **token};
                });

    rest.mapGet("/{id:int}",
                /***************************************************************************************/
                [](FromQuery<"name"> name, FromRouteInt<"id"> id) { return result{.name = *name, .id = *id}; });

    rest.run();
}