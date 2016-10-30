#ifndef ARAIGNEE_HTTP_REQUEST_H
#define ARAIGNEE_HTTP_REQUEST_H

#include <future>
#include <utility>

#include <boost/system/error_code.hpp>

#include <beast/http.hpp>

namespace araingee {
    template<typename Body, typename Header = beast::http::basic_headers<std::allocator<char>>>
    struct http_request: public beast::http::request<Body, Header> {
        template<typename... Args>
        http_request(Args... args):
            beast::http::message<false, Body, Header>{std::forward(args)...} {};
    };
}

#endif /* ARAIGNEE_HTTP_REQUEST_H */
