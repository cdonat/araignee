#ifndef ARAIGNEE_HTTP_TEMPLATED_RESPONSE_H
#define ARAIGNEE_HTTP_TEMPLATED_RESPONSE_H

#include <future>

#include <boost/system/error_code.hpp>

#include "http_response.h"

namespace araingee {
    template<typename TemplateProcessor, typename Body, typename Headers, typename AsyncWriteStream>
    class http_templated_response: public http_response<Body, Headers, AsyncWriteStream> {
        TemplateProcessor processor_;
      public:
        template<typename... Args>
        http_templated_response(TemplateProcessor processor, AsyncWriteStream&& stream, Args... args):
            http_response<Body, Headers, AsyncWriteStream>{stream, args...},
            processor_{processor} {};

        operator std::future<boost::system::error_code> () {
            return *(static_cast<http_response<Body, Headers, AsyncWriteStream>*>(processor_(this->body)));
        }
    };

    template<typename Body>
    struct null_template_processor {
        auto operator()(Body& body) -> Body& {
            return body;
        }
    };
}

#endif /* ARAIGNEE_HTTP_TEMPLATED_RESPONSE_H */

