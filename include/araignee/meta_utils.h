#ifndef ARAIGNEE_META_UTILS_H
#define ARAIGNEE_META_UTILS_H

#include <type_traits>

namespace araingee {
    template<typename FunctionType, int ParameterCount>
    struct has_param : std::false_type {};

    template<typename Ret, typename FirstParam, typename ... MoreParams>
    struct has_param<Ret(FirstParam, MoreParams...), 0> : std::true_type {};

    template<typename Ret, int ParameterCount, typename FirstParam, typename ... MoreParams>
    struct has_param<Ret(FirstParam, MoreParams...), ParameterCount> :
        has_param<Ret(MoreParams...), ParameterCount - 1> {};

    template<typename FunctionType, int ParameterCount> struct parameter_type;
    template<typename Ret, typename FirstParam, typename ... MoreParams>
    struct parameter_type<Ret(FirstParam, MoreParams...), 0> {
        using type = FirstParam;
    };
    template<typename Ret, int ParameterCount, typename FirstParam, typename ... MoreParams>
    struct parameter_type<Ret(FirstParam, MoreParams...), ParameterCount> :
        parameter_type<Ret(MoreParams...), ParameterCount - 1> {};


    template <typename, typename, typename = void>
    struct has_signature : std::false_type {};

    template <typename Func, typename Ret, typename... Args>
    struct has_signature<Func,
                         Ret(Args...),
                         typename std::enable_if<std::is_convertible<decltype(std::declval<Func>()(std::declval<Args>()...)),
                                                                     Ret>::value,
                                                 void>::type> : std::true_type {};
}

#endif /* ARAIGNEE_META_UTILS_H */

