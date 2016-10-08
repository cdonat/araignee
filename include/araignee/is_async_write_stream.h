#ifndef ARAIGNEE_IS_ASYNC_WRITE_STREAM_H
#define ARAIGNEE_IS_ASYNC_WRITE_STREAM_H

#include <type_traits>

#include <boost/asio/io_service.hpp>

#include "meta_utils.h"

namespace araingee {
    template<typename T>
    struct is_async_write_stream {
        static const bool value = std::is_member_function_pointer<decltype(&T::get_io_service)>::value &&
                                  std::is_same<typename std::result_of<decltype(&T::get_io_service)>::type, boost::asio::io_service&>::value &&
                                  !has_param<decltype(T::get_io_service), 0>::value &&
                                  std::is_member_function_pointer<decltype(&T::async_write_some)>::value &&
                                  std::is_same<typename std::result_of<decltype(&T::async_write_some)>::type, void>::value &&
                                  /*parameter_type<T::async_write_some, 0>::type,*/ // FIXME check parameter concepts
                                  !has_param<decltype(T::async_write_some), 2>::value;
    };
}

#endif /*  */
