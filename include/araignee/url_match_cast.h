#ifndef ARAIGNEE_URL_MATCH_CAST_H
#define ARAIGNEE_URL_MATCH_CAST_H

#include <string>
#include <regex>
#include <utility>

#include <boost/lexical_cast.hpp>

namespace araingee
{
    template<typename T>
    class url_match_cast {
            std::string format_;
        public:
            url_match_cast(const std::string& format) : format_{format} {};
            url_match_cast(std::string&& format) : format_{std::move(format)} {};
            url_match_cast(const url_match_cast& o) : format_{o.format_} {};
            url_match_cast(url_match_cast&& o) : format_{std::move(o.format_)} {};
            T operator () (const std::smatch& m) { return boost::lexical_cast<T>(m.format(format_)); };
    };

    template <>
    class url_match_cast<std::string> {
            std::string format_;
        public:
            url_match_cast(const std::string& format) : format_{format} {};
            url_match_cast(std::string&& format) : format_{std::move(format)} {};
            url_match_cast(const url_match_cast& o) : format_{o.format_} {};
            url_match_cast(url_match_cast&& o) : format_{std::move(o.format_)} {};
            std::string operator () (const std::smatch& m) { return m.format(format_); };
    };
}

#endif /* ARAIGNEE_URL_MATCH_CAST_H */
