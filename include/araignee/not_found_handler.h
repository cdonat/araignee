#ifndef ARAINGEE_NOT_FOUND_HANDLER_H
#define ARAINGEE_NOT_FOUND_HANDLER_H 

#include <string>
#include <type_traits>

#include <boost/system/error_code.hpp>

#include <beast/http.hpp>

#include "../araignee.h"
#include "http_response.h"

namespace araingee {
    struct not_found_handler {
        template<typename AsyncWriteStream>
        auto operator() (http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> resp)
        -> http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
            using namespace std::literals::string_literals;

            resp.status = 404;
            resp.reason = beast::http::reason_string(404);
            resp.body = "404: Not Found"s;

            resp.headers.replace("Content-Length", resp.body.size());
            resp.headers.replace("Content-Type", "text/ascii");

            return resp;
        };
    };
}

#endif /* ARAINGEE_NOT_FOUND_HANDLER_H */

