#ifndef ARAIGNEE_EXTRACT_PARAMETERS_H
#define ARAIGNEE_EXTRACT_PARAMETERS_H

#include <utility>
#include <regex>
#include <experimental/tuple>

#include "meta_utils.h"
#include "http_request.h"

namespace araingee {
    namespace {
        template <typename Derived, typename ResultType, typename ... Params> struct extract_parameters_t;

        template<typename Derived, typename ResultType, typename FirstParam, typename ... Params>
        struct extract_parameters_t<Derived, ResultType, FirstParam, Params...> :
            public extract_parameters_t<extract_parameters_t<Derived, ResultType, FirstParam, Params...>, ResultType, Params...> {
            private:
                url_match_cast<FirstParam> get_current_param_;
            public:
                extract_parameters_t(const url_match_cast<FirstParam>& get_current_param, url_match_cast<Params>&& ... get_params):
                    extract_parameters_t<extract_parameters_t<Derived, ResultType, FirstParam, Params...>, ResultType, Params...>{std::forward<url_match_cast<Params>>(get_params)...},
                    get_current_param_{get_current_param} {};

                using extract_parameters_t<extract_parameters_t<Derived, ResultType, FirstParam, Params...>, ResultType, Params...>::parse;

                template<typename UrlMatch>
                auto parse(UrlMatch&& match, Params&& ... params) -> ResultType {
                    return static_cast<Derived*>(this)->parse(std::forward<UrlMatch>(match), get_current_param_(std::forward<UrlMatch>(match)), std::forward<Params>(params)...);
                }
        };

        template<typename Derived, typename ResultType>
        struct extract_parameters_t<Derived, ResultType> {
            private:
            public:
                extract_parameters_t() = default;

                template<typename UrlMatch>
                auto parse(UrlMatch&& match) -> ResultType {
                    return static_cast<Derived*>(this)->parse(std::forward<UrlMatch>(match));
                }
        };

        template <typename ResultType, typename ... Params>
        struct extract_parameters_helper : public extract_parameters_t<extract_parameters_helper<ResultType, Params...>, ResultType, Params...> {
            extract_parameters_helper(Params&& ... params):
                extract_parameters_t<extract_parameters_helper<ResultType, Params...>, ResultType, Params...>{std::forward<Params>(params)...} {};

            using extract_parameters_t<extract_parameters_helper<ResultType, Params...>, ResultType, Params...>::parse;

            template<typename UrlMatch>
            auto parse(const UrlMatch& match, const Params& ... params) -> ResultType {
                return {params...};
            }
        };
    }

    template <typename Handler, typename = void> struct extract_parameters_chooser;

    template <typename Handler, typename AsyncWriteStream, typename ... Params>
    struct extract_parameters_chooser<Handler,
                                      typename std::enable_if<has_signature<Handler,
                                                                            std::future<boost::system::error_code>(const AsyncWriteStream&,
                                                                                                                   Params...)>::value,
                                                              void>::type> {
        static auto parse(Handler handler, const url_match_cast<Params>& ... get_params) {
            return [helper = extract_parameters_helper<std::tuple<Params...>,
                                                       Params...>{std::forward<Params>(get_params)...},
                    handler](const std::smatch& match,
                             const http_request<Body, Header>& req,
                             AsyncWriteStream&& stream) mutable -> std::future<boost::system::error_code>>::value {
                return std::experimental::apply(handler,
                                                std::tuple_cat(std::make_tuple(std::forward(stream)),
                                                               helper.parse(match)));
            };
        }
    };

    template <typename Handler, typename AsyncWriteStream, typename ... Params>
    struct extract_parameters_chooser<Handler,
                                      typename std::enable_if<has_signature<Handler,
                                                                            std::future<boost::system::error_code>(const std::smatch&,
                                                                                                                   AsyncWriteStream&, Params...)>::value,
                                                              void>::type> {
        static constexpr auto parse(Handler handler, const url_match_cast<Params>& ... get_params) {
            return [helper = extract_parameters_helper<std::tuple<Params...>,
                                                       Params...>{std::forward<Params>(get_params)...},
                    handler](const std::smatch& match,
                             const http_request<Body, Header>& req,
                             AsyncWriteStream&& stream) mutable -> std::future<boost::system::error_code>>::value {
                return std::experimental::apply(handler,
                                                std::tuple_cat(std::make_tuple(match, std::forward(stream)),
                                                               helper.parse(prefix)));
            };
        }
    };

    template <typename Handler, typename AsyncWriteStream, typename Body, typename Header, typename ... Params>
    struct extract_parameters_chooser<Handler,
                                      typename std::enable_if<has_signature<Handler,
                                                                            std::future<boost::system::error_code>(const http_request<Body, Header>&,
                                                                                                                   AsyncWriteStream&, Params...)>::value
                                                              void>::type> {
        static auto parse(Handler handler, const url_match_cast<Params>& ... get_params) {
            return [helper = extract_parameters_helper<std::tuple<Params...>,
                                                       Params...>{std::forward<Params>(get_params)...},
                    handler](const std::smatch& match,
                             const http_request<Body, Header>& req,
                             AsyncWriteStream&& stream) mutable -> std::future<boost::system::error_code>>::value {
                return std::experimental::apply(handler,
                                                std::tuple_cat(std::make_tuple(req, std::forward(stream)),
                                                               helper.parse(match)));
            };
        }
    };

    template <typename Handler, typename AsyncWriteStream, typename Body, typename Header, typename ... Params>
    struct extract_parameters_chooser<Handler,
                                      typename std::enable_if<has_signature<Handler,
                                                                            std::future<boost::system::error_code>(const std::smatch&,
                                                                                                                   const http_request<Body, Header>&,
                                                                                                                   AsyncWriteStream&, Params...)>::value,
                                                              void>::type> {
        static constexpr auto parse(Handler handler, const url_match_cast<Params>& ... get_params) {
            return [helper = extract_parameters_helper<std::tuple<Params...>,
                                                       Params...>{std::forward<Params>(get_params)...},
                    handler](const std::smatch& match,
                             const http_request<Body, Header>& req,
                             const http_request<Body, Header>& req,
                             AsyncWriteStream&& stream) mutable -> std::future<boost::system::error_code>>::value {
                return std::experimental::apply(handler,
                                                std::tuple_cat(std::make_tuple(match, req, std::forward(stream)),
                                                               helper.parse(prefix)));
            };
        }
    };

    template <typename Handler, typename ... Params>
    constexpr auto extract_parameters(Handler handler, const url_match_cast<Params>& ... get_params) {
        return extract_parameters_chooser<Handler>::parse(handler, get_params...);
    }
}

#endif /* ARAIGNEE_EXTRACT_PARAMETERS_H */

