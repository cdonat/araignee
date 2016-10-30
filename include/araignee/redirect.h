#ifndef ARAINGEE_REDIRECT_H
#define ARAINGEE_REDIRECT_H 

#include <string>
#include <utility>

#include <beast/http.hpp>

#include "../araignee.h"
#include "url_match_cast.h"
#include "http_response.h"

namespace araingee {
    namespace redirection {
        enum class method {keep, get};
        enum class persistance {temporary, permanent};

        template <method meth, persistance persist>
        class redirect_t {
            private:
                url_match_cast<std::string> format_;
            public:
                constexpr redirect_t(const std::string& format): format_{format} {};
                constexpr redirect_t(std::string&& format): format_{std::move(format)} {};

                template<typename AsyncWriteStream>
                auto operator () (const std::smatch& match,
                                  http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> resp)
                -> http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
                    using namespace std::literals::string_literals;

                    resp.status =(meth == method::keep) ?
                                     (persist == persistance::temporary ? 307 : 308) :
                                     303;
                    resp.reason = beast::http::reason_string(resp.status);

                    resp.headers.replace("Location", format_(match));
                    resp.headers.replace("Content-Type", "text/ascii");
                    resp.headers.replace("Content-Length", "0");

                    resp.body = ""s;

                    return resp;
                };
        };
    };

    template <redirection::method meth = redirection::method::keep,
              redirection::persistance persist = redirection::persistance::temporary>
    constexpr redirection::redirect_t<meth, persist> redirect(const std::string& pattern_str) {
        return {pattern_str};
    }

}

#endif /* ARAINGEE_REDIRECT_H */
