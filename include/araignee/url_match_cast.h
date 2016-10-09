#ifndef ARAIGNEE_URL_MATCH_CAST_H
#define ARAIGNEE_URL_MATCH_CAST_H

#include <string>
#include <regex>
#include <utility>

#include <boost/lexical_cast.hpp>

namespace araingee
{
    class url_match_cast_base {
        protected:
            std::string format_;
        public:
            url_match_cast_base(const std::string& format) : format_{format} {};
            url_match_cast_base(std::string&& format) : format_{std::move(format)} {};
            url_match_cast_base(const char* format) : format_{format} {};
            url_match_cast_base(const url_match_cast_base& o) : format_{o.format_} {};
            url_match_cast_base(url_match_cast_base&& o) : format_{std::move(o.format_)} {};
    };

    template<typename T>
    struct url_match_cast : public url_match_cast_base {
        template <typename P>
        url_match_cast(P&& p): url_match_cast_base{std::forward<P>(p)} {};
        T operator () (const std::smatch& m) { return boost::lexical_cast<T>(m.format(format_)); };
    };

    template <>
    struct url_match_cast<std::string> : public url_match_cast_base {
        template <typename P>
        url_match_cast(P&& p): url_match_cast_base{std::forward<P>(p)} {};
        std::string operator () (const std::smatch& m) { return m.format(format_); };
    };
}

#endif /* ARAIGNEE_URL_MATCH_CAST_H */
