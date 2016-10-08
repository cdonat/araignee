#ifndef ARAIGNEE_HTTP_RESPONSE_H
#define ARAIGNEE_HTTP_RESPONSE_H

#include <future>
#include <utility>

#include <boost/system/error_code.hpp>

#include <beast/http.hpp>

namespace araingee {
    template<typename Body, typename Headers, typename AsyncWriteStream>
    class http_response: public beast::http::request<Body, Headers> {
        AsyncWriteStream stream_;
      public:
        template<typename... Args>
        http_response(AsyncWriteStream&& stream, Args... args):
            beast::http::message<false, Body, Headers>{std::forward(args)...},
            stream_{std::forward(stream)} {};

        operator std::future<boost::system::error_code> () {
            std::promise<boost::system::error_code> p;
            auto rval = p.get_future();

            beast::http::async_write(stream_,
                                     this->body,
                                     [p{std::move(p)}](boost::system::error_code const& ec) mutable -> void {
                                        p.set_value(ec);
                                     });
            return rval;
        }
    };
}

#endif /* ARAIGNEE_HTTP_RESPONSE_H */
