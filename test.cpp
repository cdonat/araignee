#include <string>

#include <boost/asio.hpp>

#include <beast/http.hpp>

#include <araignee.h>
#include <araignee/url_match_cast.h>
#include <araignee/redirect.h>
#include <araignee/not_found_handler.h>
#include <araignee/static_files.h>
#include <araignee/http_response.h>
#include <araignee/extract_parameters.h>

#include <araignee/http_templated_response.h>

using namespace std::literals::string_literals;

namespace app {
    template<typename AsyncWriteStream>
    auto foo(araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> resp,
             int foo, std::string&& more_foo)
    -> araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
        resp.status = 200;
        resp.reason = beast::http::reason_string(200);
        
        auto foo_text = boost::lexical_cast<std::string>(foo) + "_" + more_foo;
        resp.body = "<html><head><title>foo: "s + foo_text + "</title></head><body>foo: "s + foo_text + "</body></html>"s;

        resp.headers.replace("Content-Length", resp.body.size());
        resp.headers.replace("Content-Type", "text/html");

        return resp;
    }

    class bar {
        public:
            template<typename AsyncWriteStream>
            auto operator () (araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> resp,
                              std::string&& bar_text)
            -> araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
                resp.status = 200;
                resp.reason = beast::http::reason_string(200);

                resp.headers.replace("Content-Type", "text/html");
                resp.body = "<html><head><title>bar: "s + bar_text + "</title></head><body>bar: "s + bar_text + "</body></html>"s;
                resp.headers.replace("Content-Length", resp.body.size());

                return resp;
            };
    };

    class baz {
        public:
            template<typename AsyncWriteStream>
            auto operator () (araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> resp,
                              double baz)
            -> araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
                resp.status = 200;
                resp.reason = beast::http::reason_string(200);

                resp.headers.replace("Content-Type", "text/html");
                auto baz_text = boost::lexical_cast<std::string>(baz);
                resp.body = "<html><head><title>baz: "s + baz_text + "</title></head><body>baz: "s + baz_text + "</body></html>"s;
                resp.headers.replace("Content-Length", resp.body.size());

                return resp;
            };
    };

}

int main(int, char**) {
    using namespace araingee;

    auto appRoute = route_url(route("^/foo/([0-9]+)_(.*)"s, extract_parameters(app::foo<boost::asio::ip::tcp::socket>,
                                                                               url_match_cast<int>{"$1"s},
                                                                               url_match_cast<std::string>{"asdf: $2"s}))/*,
                              route("^/bar/([a-fA-F0-9]*)"s, app::bar{"$1"s}),
                              route("^/baz/([0-9]*([.][0-9]+)?)", app::baz{"$1"s})*/);

    auto r = route_url(route("^/oldAppBase(.*)$"s, redirect("/newAppBase$1"s)),
                       route("^/newAppBase(.*)$"s, appRoute("$1"s)),
                       /*route("^/webSockets(.*)$"s, sockHandler{"$1"s}),
                       route("^/otherWebSockets(.*)$"s, otherSockHandler{"$1"s}),*/
                       route("^/static(.*)$"s, static_file_handler{boost::filesystem::current_path(), "$1"s}));

    return 0;
}


