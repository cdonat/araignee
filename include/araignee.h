#ifndef ARAIGNEE_H
#define ARAIGNEE_H

#include <string>
#include <regex>
#include <future>
#include <type_traits>
#include <utility>

#include <beast/http.hpp>

#include "araignee/meta_utils.h"
#include "araignee/is_async_write_stream.h"
#include "araignee/http_response.h"


namespace araingee {
    namespace {
        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               std::is_convertible<UrlMatch, typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               std::is_convertible<Request, typename parameter_type<RequestHandler, 1>::type>::value &&
                                                               std::is_convertible<AsyncWriteStream, typename parameter_type<RequestHandler, 2>::type>::value &&
                                                               is_async_write_stream<typename parameter_type<RequestHandler, 2>::type>::value &&
                                                               !has_param<RequestHandler, 3>::value,
                                                              RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler(std::forward(match), std::forward(req), std::forward(stream));
        }

        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               std::is_convertible<Request, typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               std::is_convertible<AsyncWriteStream, typename parameter_type<RequestHandler, 1>::type>::value &&
                                                               is_async_write_stream<typename has_param<RequestHandler, 1>::type>::value &&
                                                               !has_param<RequestHandler, 2>::value,
                                                              RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler(std::forward(req), std::forward(stream));
        }

        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               std::is_convertible<UrlMatch, typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               std::is_convertible<AsyncWriteStream, typename parameter_type<RequestHandler, 1>::type>::value &&
                                                               is_async_write_stream<typename parameter_type<RequestHandler, 1>::type>::value &&
                                                               !has_param<RequestHandler, 2>::value,
                                                              RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler(std::forward(match), std::forward(stream));
        }

        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               std::is_convertible<UrlMatch, typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               std::is_convertible<Request, typename parameter_type<RequestHandler, 1>::type>::value &&
                                                               !has_param<RequestHandler, 2>::value,
                                                              RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler(std::forward(match), std::forward(req));
        }

        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               std::is_convertible<UrlMatch, typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               !has_param<RequestHandler, 1>::value,
                                                              RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler(std::forward(match));
        }

        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               std::is_convertible<Request, typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               !has_param<RequestHandler, 1>::value,
                                                              RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler(std::forward(req));
        }

        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               std::is_convertible<AsyncWriteStream, typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               is_async_write_stream<typename parameter_type<RequestHandler, 0>::type>::value &&
                                                               !has_param<RequestHandler, 1>::value,
                                               RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler(std::forward(stream));
        }

        template<typename RequestHandler, typename UrlMatch, typename Request, typename AsyncWriteStream>
        inline auto callRequestHandler(typename std::enable_if<std::is_convertible<typename std::result_of<RequestHandler>::type, std::future<boost::system::error_code>>::value &&
                                                               !has_param<RequestHandler, 0>::value,
                                                              RequestHandler>::type&& handler,
                                       UrlMatch&& match,
                                       Request&& req,
                                       AsyncWriteStream&& stream) -> std::future<boost::system::error_code> {
            using namespace std::literals::string_literals;

            return handler();
        }
    }
   
    template<typename RequestHandler> class route_t {
        const std::regex pattern_;
        RequestHandler requestHandler_;
        public:
            route_t(const std::regex& pattern, RequestHandler&& requestHandler):
                pattern_{pattern},
                requestHandler_{std::forward<RequestHandler>(requestHandler)} {};

            std::smatch operator == (const std::string& path) const {
                std::smatch r;
                std::regex_match(path, r, pattern_);
                return r;
            };

            template<typename UrlMatch, typename Request, typename AsyncWriteStream>
            std::future<boost::system::error_code> operator() (UrlMatch&& match, Request&& req, AsyncWriteStream&& stream) {
                return callRequestHandler(requestHandler_, match, req, stream);
            };
    };

    template<typename RouteUrl>
    class route_url_request_handler {
        std::string format_;
        RouteUrl& ruh_;

        public:
            route_url_request_handler(const std::string& format, RouteUrl& ruh):
                format_{format},
                ruh_{ruh} {};

            template<typename Request, typename AsyncWriteStream>
            std::future<boost::system::error_code> operator() (const std::smatch& match, const Request& req, AsyncWriteStream& stream) {
                return ruh_(match.format(format_), req, stream);
            }
    };

    template<typename... Routes> class route_url_t;
    template<typename FirstRoute, typename... Routes>
    class route_url_t<FirstRoute, Routes...> {
            FirstRoute firstRoute_;
            route_url_t<Routes...> routes_;
        public:
            route_url_t(FirstRoute&& firstRoute, Routes&&... routes):
                firstRoute_{std::forward<FirstRoute>(firstRoute)},
                routes_{std::forward<Routes>(routes)...} {};

            route_url_request_handler<route_url_t> operator() (const std::string& format) {
                return route_url_request_handler<route_url_t>(format, *this);
            }
     
            template<typename Request, typename AsyncWriteStream>
            std::future<boost::system::error_code> operator() (const std::string& url, const Request& req, AsyncWriteStream& stream) {
                auto match = (firstRoute_ == url);
                if(!match.empty()) {
                    return firstRoute_(match, req, stream);
                } else {
                    return routes_(url, req, stream);
                }
            };
    };

    template<>
    class route_url_t<> {
        public:
            route_url_t() {};

            /*route_url_request_handler<Routes...> operator() (const std::string& format) {
                return route_url_request_handler<route_url_t, Routes...>(format, *this);
            }*/
     
            template<typename Request, typename AsyncWriteStream>
            std::future<boost::system::error_code> operator() (const std::string& url, const Request& req, AsyncWriteStream& stream) {
                using namespace std::literals::string_literals;

                auto rval = http_response<beast::http::string_body, beast::http::headers, AsyncWriteStream>{stream};
                rval.status = 404;
                rval.reason = beast::http::reason_string(404);
                rval.body = "404: Not Found"s;

                return rval;
            };
    };

    template<typename Handler>
    route_t<Handler> route(const std::regex& pattern, Handler&& handler) {
        return route_t<Handler>{pattern, std::forward<Handler>(handler)};
    }

    template<typename Handler>
    route_t<Handler> route(const std::string& pattern, Handler&& handler) {
        return route_t<Handler>{std::regex{pattern}, std::forward<Handler>(handler)};
    }


    template<typename... Routes>
    route_url_t<Routes...> route_url(Routes&&... routes) {
        return route_url_t<Routes...>{std::forward<Routes>(routes)...};
    }
}

#endif /* ARAIGNEE_H */

