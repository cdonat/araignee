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
        http_templated_response(const TemplateProcessor& processor,
                                const http_response<Body, Headers, AsyncWriteStream>& resp):
            http_response<Body, Headers, AsyncWriteStream>{resp},
            processor_{processor} {};

        template<typename... Args>
        http_templated_response(const TemplateProcessor& processor, AsyncWriteStream&& stream, Args... args):
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

