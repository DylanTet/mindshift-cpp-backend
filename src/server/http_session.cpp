#include "http_session.h"
#include <chrono>
#include <iostream>
#include <memory>

HttpSession::HttpSession(tcp::socket &&socket, std::shared_ptr<Router> router)
    : stream_(std::move(socket)), router_(router) {

  stream_.expires_never();
}

void HttpSession::run() {
  net::dispatch(
      stream_.get_executor(),
      beast::bind_front_handler(&HttpSession::do_read, shared_from_this()));
}

void HttpSession::do_read() {
  req_.clear();

  stream_.expires_after(std::chrono::seconds(30));

  http::async_read(
      stream_, buffer_, req_,
      beast::bind_front_handler(&HttpSession::on_read, shared_from_this()));
}

template <class Body, class Fields>
void HttpSession::send_response(http::response<Body, Fields> &&res) {
  // Store response in shared_ptr to keep it alive during async write???
  auto sp = std::make_shared<http::response<Body, Fields>>(std::move(res));
  res_ = sp;

  sp->set(http::field::access_control_allow_origin, "*");
  sp->set(http::field::access_control_allow_methods,
          "GET, POST, PUT, DELETE, OPTIONS");
  sp->set(http::field::access_control_allow_headers,
          "Content-Type, Authorization");

  http::async_write(stream_, *sp,
                    beast::bind_front_handler(&HttpSession::on_write,
                                              shared_from_this(),
                                              sp->need_eof()));
}

void HttpSession::on_write(bool close, beast::error_code ec,
                           size_t bytes_transferred) {

  boost::ignore_unused(bytes_transferred);

  if (ec) {
    throw beast::system_error{ec};
  }

  if (close) {
    return do_close();
  }

  res_->clear();

  do_read();
}

void HttpSession::on_read(beast::error_code ec, size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec == http::error::end_of_stream) {
    return do_close();
  }

  if (ec) {
    throw std::runtime_error("HTTP Read Error" + ec.message());
  }

  std::cout << "Received " << req_.method_string() << "request for "
            << req_.target() << std::endl;

  try {
    auto response = router_->handleRequest(req_);
    send_response(std::move(response));
  } catch (const std::exception &e) {
    std::cerr << "Error processing request: " << e.what() << std::endl;

    // Send error response
    http::response<http::string_body> error_res{
        http::status::internal_server_error, req_.version()};

    error_res.set(http::field::server, "Beast HTTP Server");
    error_res.set(http::field::content_type, "application/json");
    error_res.body() = R"({"error": "Internal Server Error"})";
    error_res.prepare_payload();

    send_response(std::move(error_res));
  }
}

void HttpSession::do_close() {
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
  // Connection was closed gracefully
}
