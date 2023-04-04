#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "Engine/Context.hpp"
#include "Engine/Endpoint.hpp"
#include "Engine/IContext.hpp"
#include "Engine/IEndpoint.hpp"
#include "Http/HttpMethod.hpp"
#include "Router/Exceptions.hpp"
#include "Router/Router.hpp"

namespace
{
    struct EndpointData
    {
        std::string path;
        sd::HttpMethod method = sd::HttpMethod::Get;
        sd::Action action = [](sd::IContext &) {};
    };

    using Datas = std::vector<EndpointData>;

    Datas generateNestedEndpoints(std::string path, size_t num)
    {
        if (num == 0)
        {
            return {{path}};
        }
        Datas result{generateNestedEndpoints(path + "/" + std::to_string(num), num - 1)};
        result.push_back({path});
        return result;
    }
} // namespace
class RouterTest : public ::testing::Test
{
  protected:
    static void TearUpTestSuite() {}
    sd::Router router;

    RouterTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~RouterTest() {}

    void addEndpoints(Datas datas)
    {
        for (auto &data : datas)
        {
            router.addEndpoint(std::make_unique<sd::Endpoint>(data.method, data.path, data.action));
        }
    }

    bool shouldMatch(const EndpointData &data, std::string_view path, sd::HttpMethod method = sd::HttpMethod::Get) const
    {
        if (auto endpoint = router.match(method, path))
        {
            return endpoint->getRouteTemplate() == data.path && endpoint->getHttpMethod() == data.method;
        }
        return false;
    }

    bool shouldNotMatch(std::string_view path, sd::HttpMethod method = sd::HttpMethod::Get) const
    {
        return !router.match(method, path);
    }

    static void TearDownTestSuite() {}
};

TEST_F(RouterTest, SimpleMatch)
{
    Datas datas = {{"/api/users/bob"},  {"/api/users/bob/cmok"}, {"/api/users/bob/cmok/fok"},
                   {"/api/users/name"}, {"/api/users/man"},      {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/bob/cmok"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/bob/cmok/fok"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/name"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[5], "/api"));
}

TEST_F(RouterTest, NotFound)
{
    router.compile();

    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/cmok/fok"));
    EXPECT_TRUE(shouldNotMatch("/api"));
    EXPECT_TRUE(shouldNotMatch("/"));
}

TEST_F(RouterTest, AlreadyRegistered)
{
    Datas datas = {{"/api/users/man"}, {"/api/users/man"}, {"/api"}};

    addEndpoints(datas);

    EXPECT_THROW(router.compile(), sd::EndpointAlreadyRegistered);
}

TEST_F(RouterTest, LargeTest)
{
    Datas datas = generateNestedEndpoints("/api/users", 50);

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], datas[0].path)); // /api/users/50/49/48...
}

TEST_F(RouterTest, IntConstrainTest)
{
    Datas datas = {{"/api/users/{id:int}/bob"},
                   {"/api/users/{id:int}/bob/{id:int}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/12/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/12/bob/1234"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/i12123asd"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/as3asd"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/asd0"));
}

TEST_F(RouterTest, BoolConstrainTest)
{
    Datas datas = {{"/api/users/{id:bool}/bob"},
                   {"/api/users/{id:bool}/bob/{id:bool}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1234/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/-1234/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/0/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/TRUE/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/FALSE/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/tRue/bob/FaLse"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/-1/bob/FaLse"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/asd0"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/non"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/bub"));
}

TEST_F(RouterTest, FloatConstrainTest)
{
    Datas datas = {{"/api/users/{id:float}/bob"},
                   {"/api/users/{id:float}/bob/{id:float}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/12.1/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/123/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1.2/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/-12.0/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/0.111/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/12.21/bob/0.1234"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/bubwe000"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/i23e000"));
}

TEST_F(RouterTest, DoubleConstrainTest)
{
    Datas datas = {{"/api/users/{id:double}/bob"},
                   {"/api/users/{id:double}/bob/{id:double}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/12.1/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/123/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1.2/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/-12.0/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/0.111/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/12.21/bob/0.1234"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/bubwe000"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/io123WD000"));
}

TEST_F(RouterTest, AnyConstrainTest)
{
    Datas datas = {{"/api/users/{id}/bob"},
                   {"/api/users/{id:string}/bob/{id:any}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/pokpok/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/testtt/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1.2/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/-12.0/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/0.111/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/12.21/bob/aosdijasodi"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
}

TEST_F(RouterTest, AlphaConstrainTest)
{
    Datas datas = {{"/api/users/{id:alpha}/bob"},
                   {"/api/users/{id:alpha}/bob/{id:alpha}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/pokpok/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/testtt/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/asdqw/bob/aosdijasodi"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/bubwe000"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/123e000"));
}

TEST_F(RouterTest, RequiredConstrainTest)
{
    Datas datas = {{"/api/users/{id:required}/bob"},
                   {"/api/users/{id:required}/bob/{id:required}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/pokpok/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/testtt/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/asdqw/bob/aosdijasodi"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users//bob/bubwe000"));
}

TEST_F(RouterTest, MinConstrainTest)
{
    Datas datas = {{"/api/users/{id:min(12)}/bob"},
                   {"/api/users/{id:min(12)}/bob/{id:min(0)}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/13/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/14/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/13/bob/1"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1/bob/1"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1/bob/-1"));
    EXPECT_TRUE(shouldNotMatch("/api/users/13/bob/-1"));
}

TEST_F(RouterTest, MaxConstrainTest)
{
    Datas datas = {{"/api/users/{id:max(12)}/bob"},
                   {"/api/users/{id:max(12)}/bob/{id:max(0)}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/2/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/3/bob/-1"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1/bob/1"));
    EXPECT_TRUE(shouldNotMatch("/api/users/112/bob/-1"));
    EXPECT_TRUE(shouldNotMatch("/api/users/10/bob/11"));
}

TEST_F(RouterTest, RangeConstrainTest)
{
    Datas datas = {{"/api/users/{id:range(2,3)}/bob"},
                   {"/api/users/{id:range(2,3)}/bob/{id:range(1,2)}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/2/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/3/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/2/bob/1"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1/bob/132"));
    EXPECT_TRUE(shouldNotMatch("/api/users/132/bob/123"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1033/bob/11"));
}

TEST_F(RouterTest, MaxLengthConstrainTest)
{
    Datas datas = {{"/api/users/{id:maxlength(3)}/bob"},
                   {"/api/users/{id:maxlength(3)}/bob/{id:maxlength(2)}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/12/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/12/bob/1"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/122/bob/132"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1233/bob/1"));
    EXPECT_TRUE(shouldNotMatch("/api/users/10/bob/112"));
}

TEST_F(RouterTest, MinLengthConstrainTest)
{
    Datas datas = {{"/api/users/{id:minlength(3)}/bob"},
                   {"/api/users/{id:minlength(3)}/bob/{id:minlength(2)}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/120/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/123/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/1212/bob/12"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/12/bob/132"));
    EXPECT_TRUE(shouldNotMatch("/api/users/132/bob/1"));
    EXPECT_TRUE(shouldNotMatch("/api/users/10/bob/11"));
}

TEST_F(RouterTest, LengthConstrainTest)
{
    Datas datas = {{"/api/users/{id:length(2,3)}/bob"},
                   {"/api/users/{id:length(2,3)}/bob/{id:length(1,2)}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/12/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/123/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/32/bob/12"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1/bob/132"));
    EXPECT_TRUE(shouldNotMatch("/api/users/132/bob/123"));
    EXPECT_TRUE(shouldNotMatch("/api/users/1033/bob/11"));
}

TEST_F(RouterTest, RegexConstrainTest)
{
    Datas datas = {{"/api/users/{id:regex(\\d+)}/bob"},
                   {"/api/users/{id:regex(\\d+)}/bob/{id:regex(\\w+)}"},
                   {"/api/users/{name}"},
                   {"/api/users/man"},
                   {"/api"}};

    addEndpoints(datas);
    router.compile();

    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/12/bob"));
    EXPECT_TRUE(shouldMatch(datas[0], "/api/users/1/bob"));
    EXPECT_TRUE(shouldMatch(datas[1], "/api/users/12/bob/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/hello"));
    EXPECT_TRUE(shouldMatch(datas[2], "/api/users/1234123"));
    EXPECT_TRUE(shouldMatch(datas[3], "/api/users/man"));
    EXPECT_TRUE(shouldMatch(datas[4], "/api"));
    EXPECT_TRUE(shouldNotMatch("/api/users/hwllo/bob/132"));
    EXPECT_TRUE(shouldNotMatch("/api/users/asd/bob/1"));
}
