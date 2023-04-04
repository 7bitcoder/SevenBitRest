#pragma once

#include "boost/url/url.hpp"
#include <boost/container_hash/hash.hpp>
#include <string>

namespace sd
{
    struct Url
    {
        std::string host = "localhost";
        uint16_t port = 9090;
        bool useSsl = false;

        Url(std::string url)
        {
            boost::urls::url urlView{url};
            if (urlView.has_scheme())
            {
                useSsl = urlView.scheme_id() == boost::urls::scheme::https;
                port = urlView.scheme_id() == boost::urls::scheme::https ? 443 : 80;
            }
            if (urlView.has_port())
            {
                port = urlView.port_number();
            }
            host = urlView.host();
        }

        std::string toString()
        {
            boost::urls::url urlView;
            urlView.set_scheme_id(useSsl ? boost::urls::scheme::https : boost::urls::scheme::http);
            urlView.set_port_number(port);
            urlView.set_host(host);
            return std::string{urlView.buffer()};
        }
    };

    inline bool operator==(const Url &l, const Url &r)
    {
        return l.useSsl == r.useSsl && l.host == r.host && l.port == r.port;
    }
} // namespace sd

template <> struct std::hash<sd::Url>
{
    std::size_t operator()(const sd::Url &url) const
    {
        std::size_t result = 0;
        boost::hash_combine(result, url.host);
        boost::hash_combine(result, url.port);
        boost::hash_combine(result, url.useSsl);
        return result;
    }
};