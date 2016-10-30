#ifndef ARAIGNEE_H
#define ARAIGNEE_H

#include <string>
#include <regex>
#include <future>
#include <type_traits>
#include <utility>

#include <beast/http.hpp>

#include "araignee/meta_utils.h"
#include "araignee/http_response.h"
#include "araignee/http_request.h"


namespace araingee {
    namespace {
        template<typename RequestHandler, typename AsyncWriteStream, typename Body, typename Header>
        inline auto callRequestHandler(typename std::enable_if<has_signature<RequestHandler,
                                                                             std::future<boost::system::error_code>(const std::smatch&,
                                                                                                                    const http_request<Body, Header>&,
                                                                                                                    AsyncWriteStream&)>::value,
                                                               RequestHandler>::type&& handler,
                                       const std::smatch& match,
                                       const http_request<Body, Header>& req,
                                       AsyncWriteStream& stream) -> std::future<boost::system::error_code> {
            return handler(match, std::forward(req), std::forward(stream));
        }

        template<typename RequestHandler, typename AsyncWriteStream, typename Body, typename Header>
        inline auto callRequestHandler(typename std::enable_if<has_signature<RequestHandler,
                                                                             std::future<boost::system::error_code>(const http_request<Body, Header>&,
                                                                                                                    AsyncWriteStream&)>::value,
                                                               RequestHandler>::type&& handler,
                                       const std::smatch& match,
                                       const http_request<Body, Header>& req,
                                       AsyncWriteStream& stream) -> std::future<boost::system::error_code> {
            return handler(req, std::forward(stream));
        }

        template<typename RequestHandler, typename AsyncWriteStream, typename Body, typename Header>
        inline auto callRequestHandler(typename std::enable_if<has_signature<RequestHandler,
                                                                             std::future<boost::system::error_code>(const std::smatch&,
                                                                                                                    AsyncWriteStream&)>::value,
                                                               RequestHandler>::type&& handler,
                                       const std::smatch& match,
                                       const http_request<Body, Header>& req,
                                       AsyncWriteStream& stream) -> std::future<boost::system::error_code> {
            return handler(match, std::forward(stream));
        }

        template<typename RequestHandler, typename AsyncWriteStream, typename Body, typename Header>
        inline auto callRequestHandler(typename std::enable_if<has_signature<RequestHandler,
                                                                             std::future<boost::system::error_code>(AsyncWriteStream&)>::value,
                                               RequestHandler>::type&& handler,
                                       const std::smatch& match,
                                       const http_request<Body, Header>& req,
                                       AsyncWriteStream& stream) -> std::future<boost::system::error_code> {
            return handler(std::forward(stream));
        }
    }
   
    template<typename RequestHandler> class route_t {
        const std::regex pattern_;
        RequestHandler requestHandler_;
        public:
            constexpr route_t(const std::regex& pattern, RequestHandler&& requestHandler):
                pattern_{pattern},
                requestHandler_{std::forward<RequestHandler>(requestHandler)} {};

            std::smatch operator == (const std::string& path) const {
                std::smatch r;
                std::regex_match(path, r, pattern_);
                return r;
            };

            template<typename AsyncWriteStream, typename Body, typename Header>
            std::future<boost::system::error_code> operator() (const std::smatch& match,
                                                               const http_request<Body, Header>& req,
                                                               AsyncWriteStream& stream) {
                return callRequestHandler(requestHandler_, match, req, stream);
            };
    };

    template<typename RouteUrl>
    class route_url_request_handler {
        std::string format_;
        RouteUrl& ruh_;

        public:
            constexpr route_url_request_handler(const std::string& format, RouteUrl& ruh):
                format_{format},
                ruh_{ruh} {};

            template<typename AsyncWriteStream, typename Body, typename Header>
            std::future<boost::system::error_code> operator() (const std::smatch& match,
                                                               const http_request<Body, Header>& req,
                                                               AsyncWriteStream& stream) {
                return ruh_(match.format(format_), req, stream);
            }
    };

    template<typename... Routes> class route_url_t;
    template<typename FirstRoute, typename... Routes>
    class route_url_t<FirstRoute, Routes...> {
            FirstRoute firstRoute_;
            route_url_t<Routes...> routes_;
        public:
            constexpr route_url_t(FirstRoute&& firstRoute, Routes&&... routes):
                firstRoute_{std::forward<FirstRoute>(firstRoute)},
                routes_{std::forward<Routes>(routes)...} {};

            constexpr route_url_request_handler<route_url_t> operator() (const std::string& format) {
                return route_url_request_handler<route_url_t>(format, *this);
            }
     
            template<typename AsyncWriteStream, typename Body, typename Header>
            std::future<boost::system::error_code> operator() (const http_request<Body, Header>& req,
                                                               AsyncWriteStream& stream) {
                auto match = (firstRoute_ == req.url);
                if(!match.empty()) {
                    return firstRoute_(match, req, stream);
                } else {
                    return routes_(req, stream);
                }
            };
    };

    template<>
    class route_url_t<> {
        public:
            constexpr route_url_t() {};

            template<typename AsyncWriteStream, typename Body, typename Header>
            std::future<boost::system::error_code> operator() (const http_request<Body, Header>& req,
                                                               http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream> resp) {
                using namespace std::literals::string_literals;

                resp.status = 404;
                resp.reason = beast::http::reason_string(404);
                resp.body = "404: Not Found"s;

                return resp;
            };
    };

    template<typename Handler>
    constexpr route_t<Handler> route(const std::regex& pattern, Handler&& handler) {
        return route_t<Handler>{pattern, std::forward<Handler>(handler)};
    }

    template<typename Handler>
    constexpr route_t<Handler> route(const std::string& pattern, Handler&& handler) {
        return route_t<Handler>{std::regex{pattern}, std::forward<Handler>(handler)};
    }


    template<typename... Routes>
    constexpr route_url_t<Routes...> route_url(Routes&&... routes) {
        return route_url_t<Routes...>{std::forward<Routes>(routes)...};
    }
}

#endif /* ARAIGNEE_H */

