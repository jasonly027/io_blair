#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/system.hpp>

namespace io_blair {
// NOLINTBEGIN(misc-unused-alias-decls)
namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
// NOLINTEND(misc-unused-alias-decls)

using net::ip::tcp;
using error_code = boost::system::error_code;
}  // namespace io_blair
