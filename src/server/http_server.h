#pragma once

#include "http_session.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Router;

class Server {
private:
  net::io_context ioc_;
  tcp::acceptor acceptor_;
  std::shared_ptr<Router> router_;
  std::vector<std::thread> threads_;
  const std::string address_;
  unsigned short port_;
  int thread_count_;
  std::shared_ptr<HttpSession> session_;

public:
  Server(const std::string &address = "0.0.0.0", unsigned short port = 8000,
         int threads = std::thread::hardware_concurrency());

  ~Server();

  void set_router(std::shared_ptr<Router> router);
  void start();
  void stop();

private:
  void do_accept();
  void on_accept(beast::error_code ec, tcp::socket socket);
};
