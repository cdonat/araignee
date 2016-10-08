#include <string>
#include <beast/http.hpp>

#include <araignee.h>
#include <araignee/url_match_cast.h>
#include <araignee/redirect.h>
#include <araignee/not_found_handler.h>
#include <araignee/static_files.h>
#include <araignee/http_response.h>

#include <araignee/http_templated_response.h>

using namespace std::literals::string_literals;

namespace app {
    class foo {
        private:
            araingee::url_match_cast<int> get_foo_;
        public:
            foo(const araingee::url_match_cast<int>& get_foo): get_foo_{get_foo} {};
            foo(araingee::url_match_cast<int>&& get_foo): get_foo_{std::move(get_foo)} {};

            template<typename AsyncWriteStream>
            auto operator () (const std::smatch& match, const AsyncWriteStream& s)
            -> araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
                auto resp = araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream>{s};
                resp.status = 200;
                resp.reason = beast::http::reason_string(200);
                
                auto foo_text = boost::lexical_cast<std::string>(get_foo_(match));
                resp.body = "<html><head><title>foo: "s + foo_text + "</title></head><body>foo: "s + foo_text + "</body></html>"s;

                resp.headers.replace("Content-Length", resp.body.size());
                resp.headers.replace("Content-Type", "text/html");

                return resp;
            }
    };

    class bar {
        private:
            araingee::url_match_cast<std::string> get_bar_;
        public:
            bar(const araingee::url_match_cast<std::string>& get_bar): get_bar_{get_bar} {};
            bar(araingee::url_match_cast<std::string>&& get_bar): get_bar_{std::move(get_bar)} {};

            template<typename AsyncWriteStream>
            auto operator () (const std::smatch& match, const AsyncWriteStream& s)
            -> araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
                auto resp = araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream>{s};
                resp.status = 200;
                resp.reason = beast::http::reason_string(200);

                resp.headers.replace("Content-Type", "text/html");
                auto bar_text = get_bar_(match);
                resp.body = "<html><head><title>bar: "s + bar_text + "</title></head><body>bar: "s + bar_text + "</body></html>"s;
                resp.headers.replace("Content-Length", resp.body.size());

                return resp;
            };
    };

    class baz {
        private:
            araingee::url_match_cast<double> get_baz_;
        public:
            baz(const araingee::url_match_cast<double>& get_baz): get_baz_{get_baz} {};
            baz(araingee::url_match_cast<double>&& get_baz): get_baz_{std::move(get_baz)} {};

            template<typename AsyncWriteStream>
            auto operator () (const std::smatch& match, const AsyncWriteStream& s)
            -> araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> {
                auto resp = araingee::http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream>{s};
                resp.status = 200;
                resp.reason = beast::http::reason_string(200);

                resp.headers.replace("Content-Type", "text/html");
                auto baz_text = boost::lexical_cast<std::string>(get_baz_(match));
                resp.body = "<html><head><title>baz: "s + baz_text + "</title></head><body>baz: "s + baz_text + "</body></html>"s;
                resp.headers.replace("Content-Length", resp.body.size());

                return resp;
            };
    };

}

int main(int, char**) {
    using namespace araingee;

    auto appRoute = route_url(route("^/foo/([0-9]+)"s, app::foo{url_match_cast<int>{"$1"s}}),
                              route("^/bar/([a-fA-F0-9]*)"s, app::bar{url_match_cast<std::string>{"$1"s}}),
                              route("^/baz/([0-9]*([.][0-9]+)?)", app::baz{url_match_cast<double>{"$1"}}));

    auto r = route_url(route("^/oldAppBase(.*)$"s, redirect("/newAppBase$1"s)),
                       route("^/newAppBase(.*)$"s, appRoute("$1"s)),
                       /*route("^/webSockets(.*)$"s, sockHandler{"$1"s}),
                       route("^/otherWebSockets(.*)$"s, otherSockHandler{"$1"s}),*/
                       route("^/static(.*)$"s, static_file_handler{boost::filesystem::current_path(), "$1"s}));

    return 0;
}


