#pragma once

#include <string>
#include <vector>

namespace sd
{
    enum Protocol
    {
        Http1 = 1,
        Http2 = 2,
        Http3 = 4
    };

    struct ServerSettings
    {
        Protocol protocol = Protocol::Http2;
        std::vector<std::string> urls;
        uint16_t threadsNumber = 0;
        size_t timeoutSec = 30;
        size_t bodyLimit = 31'457'280; // 30 MB
    };
} // namespace sd