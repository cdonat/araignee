#ifndef ARAINGEE_STATIC_FILES_H
#define ARAINGEE_STATIC_FILES_H 

#include <string>
#include <future>
#include <fstream>
#include <exception>

#include <boost/filesystem.hpp>

#include <beast/http/streambuf_body.hpp>
#include <boost/system/error_code.hpp>

#include <beast/http.hpp>

#include "../araignee.h"
#include "url_match_cast.h"
#include "http_response.h"

namespace araingee {
    class static_file_handler {
        boost::filesystem::path basePath_;
        std::string format_;

        template<typename AsyncWriteStream>
        std::future<boost::system::error_code> internal_error(http_response<beast::http::string_body,
                                                              beast::http::headers, AsyncWriteStream> resp,
                                                              const std::string& message = "") {
            using namespace std::literals::string_literals;

            resp.status = 500;
            resp.reason = beast::http::reason_string(500);
            resp.body = "500: Internal Server Error"s + ((message.size() == 0) ? ""s : ": "s + message);

            resp.headers.replace("Content-Length", resp.body.size());
            resp.headers.replace("Content-Type", "text/ascii");

            return resp;
        };

        public:
            static_file_handler(const boost::filesystem::path& basePath, const std::string& format):
                basePath_{boost::filesystem::absolute(basePath)},
                format_{format} {};

            template<typename Request, typename AsyncWriteStream>
            std::future<boost::system::error_code> operator() (const std::smatch& match,
                                                               const Request& req,
                                                               AsyncWriteStream& stream) {
                try {
                    auto path = boost::filesystem::canonical(boost::filesystem::path{araingee::url_match_cast<std::string>{format_}(match)},
                                                             basePath_);
                    if( boost::filesystem::exists(path) ) {
                        // TODO: check if path is below basePath_.
                        
                        auto filestream = std::ifstream{path.native()};
                        auto resp = http_response<beast::http::streambuf_body, beast::http::headers, AsyncWriteStream>{stream,
                                                                                                                       filestream.rdbuf()};
                        resp.status = 200;
                        resp.reason = beast::http::reason_string(200);

                        auto file_size = boost::filesystem::file_size(path);
                        resp.headers.replace("Content-Lenth", file_size);

                        return resp;
                    }
                } catch(std::exception& e) {
                    return internal_error(stream, e.what());
                } catch(...) {
                    return internal_error(stream);
                }
                return not_found_handler{}(stream);
            };
    };

}

#endif /* ARAINGEE_STATIC_FILES_H */
