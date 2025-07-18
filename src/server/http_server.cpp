#include "http_server.h"
#include "http_session.h"
#include <iostream>
#include <memory>
#include <stdexcept>

Server::Server(const std::string &address, unsigned short port, int threads)
    : ioc_{threads}, acceptor_(net::make_strand(ioc_)), port_(port),
      thread_count_(threads) {}

Server::~Server() { stop(); }

void Server::set_router(std::shared_ptr<Router> router) { router_ = router; }

void Server::start() {
  beast::error_code ec;
  auto const endpoint = tcp::endpoint{net::ip::make_address(address_), port_};
  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    throw std::runtime_error("Error creating endpoint: " + ec.message());
  }

  acceptor_.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    throw std::runtime_error("Error setting socket option: " + ec.message());
  }

  acceptor_.bind(endpoint, ec);
  if (ec) {
    throw std::runtime_error("Error binding socket: " + ec.message());
  }

  acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    throw std::runtime_error("Error listening on endpoint" + ec.message());
  }

  std::cout << "Server listening on: " << address_ << ":" << port_ << std::endl;

  do_accept();

  threads_.reserve(thread_count_);
  for (int i = 0; i < thread_count_; ++i) {
    threads_.emplace_back([this] { ioc_.run(); });
  }
}

void Server::stop() {
  ioc_.stop();
  for (auto &t : threads_) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void Server::do_accept() {
  acceptor_.async_accept(net::make_strand(ioc_),
                         beast::bind_front_handler(&Server::on_accept, this));
}

void Server::on_accept(beast::error_code ec, tcp::socket socket) {
  if (ec) {
    throw std::runtime_error("Accept error: " + ec.message());
  } else {
    session_ = std::make_shared<HttpSession>(std::move(socket), router_);
    session_->run();
  }

  do_accept();
}
