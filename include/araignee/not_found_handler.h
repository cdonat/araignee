#ifndef ARAINGEE_NOT_FOUND_HANDLER_H
#define ARAINGEE_NOT_FOUND_HANDLER_H 

#include <string>
#include <type_traits>

#include <boost/system/error_code.hpp>

#include <beast/http.hpp>

#include "../araignee.h"
#include "is_async_write_stream.h"
#include "http_response.h"

namespace araingee {
    struct not_found_handler {
        template<typename AsyncWriteStream>
        auto operator() (typename std::enable_if<is_async_write_stream<AsyncWriteStream>::value, AsyncWriteStream>::type stream)
        -> http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
            using namespace std::literals::string_literals;

            auto rval = http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream>{stream};
            rval.status = 404;
            rval.reason = beast::http::reason_string(404);
            rval.body = "404: Not Found"s;

            rval.headers.replace("Content-Length", rval.body.size());
            rval.headers.replace("Content-Type", "text/ascii");

            return rval;
        };
    };
}

#endif /* ARAINGEE_NOT_FOUND_HANDLER_H */

