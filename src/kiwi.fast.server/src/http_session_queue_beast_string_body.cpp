#include <kiwi.fast.server/detail/config.h>

#include <kiwi.fast.server/src/http_session.h>

#include <functional>

void http_session::queue::operator()(boost::beast::http::message<false,boost::beast::http::basic_string_body<char8_t>,boost::beast::http::fields> msg)
{
    // Allocate and store the work
    items_.push_back(boost::make_unique<work_impl<false,boost::beast::http::basic_string_body<char8_t>,boost::beast::http::fields>>(self_, std::move(msg)));

    // If there was no previous work, start this one
    if(items_.size() == 1)
        (*items_.front())();
}
