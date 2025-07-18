#pragma once

#include "router.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = beast::net;
using tcp = boost::asio::ip::tcp;

class Router;

class HttpSession : public std::enable_shared_from_this<HttpSession> {

public:
  HttpSession(tcp::socket &&socket, std::shared_ptr<Router> router);
  void run();

private:
  void do_read();
  void on_read(beast::error_code ec, size_t bytes_transferred);
  void on_write(bool close, beast::error_code ec, size_t bytes_transferred);
  void do_close();

private:
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  http::request<http::string_body> req_;
  std::shared_ptr<Router> router_;
  std::shared_ptr<HttpResponse> res_;

  template <class Body, class Fields>
  void send_response(http::response<Body, Fields> &&res);
};
