#ifndef ARAIGNEE_META_UTILS_H
#define ARAIGNEE_META_UTILS_H

namespace araingee {
    template<typename FunctionType, int ParameterCount>
    struct has_param {
        static const bool value = false;
    };
    template<typename Ret, typename FirstParam, typename ... MoreParams>
    struct has_param<Ret(FirstParam, MoreParams...), 0> {
        static const bool value = true;
    };
    template<typename Ret, int ParameterCount, typename FirstParam, typename ... MoreParams>
    struct has_param<Ret(FirstParam, MoreParams...), ParameterCount>  {
        static const bool value = has_param<Ret(MoreParams...), ParameterCount - 1>::value;
    };

    template<typename FunctionType, int ParameterCount> struct parameter_type;
    template<typename Ret, typename FirstParam, typename ... MoreParams>
    struct parameter_type<Ret(FirstParam, MoreParams...), 0> {
        using type = FirstParam;
    };
    template<typename Ret, int ParameterCount, typename FirstParam, typename ... MoreParams>
    struct parameter_type<Ret(FirstParam, MoreParams...), ParameterCount>  {
        using type = typename parameter_type<Ret(MoreParams...), ParameterCount - 1>::type;
    };
}

#endif /* ARAIGNEE_META_UTILS_H */

