#include "router.h"
#include "auth.h"
#include <boost/beast/http.hpp>
#include <exception>
#include <iostream>
#include <regex>
#include <string_view>

Router::Router() {
  not_found_handler_ = [](const HttpRequest &req) {
    HttpResponse res{http::status::not_found, req.version()};
    res.set(http::field::server, "HTTP Server");
    res.set(http::field::content_type, "application/json");
    res.body() =
        R"({"error": "Not Found", "message": "The requested resource was not found"})";
    res.prepare_payload();
    return res;
  };

  error_handler_ = [](const HttpRequest &req) {
    HttpResponse res{http::status::internal_server_error, req.version()};
    res.set(http::field::server, "Beast HTTP Server");
    res.set(http::field::content_type, "application/json");
    res.body() =
        R"({"error": "Internal Server Error", "message": "An unexpected error occurred"})";
    res.prepare_payload();
    return res;
  };
}

void Router::addRoute(http::verb method, const std::string &pattern,
                      RouteHandler handler) {
  routes_.push_back({pattern, std::move(handler), method});
}

void Router::get(const std::string &pattern, RouteHandler handler) {
  addRoute(http::verb::get, pattern, std::move(handler));
}

void Router::post(const std::string &pattern, RouteHandler handler) {
  addRoute(http::verb::post, pattern, std::move(handler));
}

void Router::put(const std::string &pattern, RouteHandler handler) {
  addRoute(http::verb::put, pattern, std::move(handler));
}

void Router::delete_(const std::string &pattern, RouteHandler handler) {
  addRoute(http::verb::delete_, pattern, std::move(handler));
}

HttpResponse Router::handleRequest(const HttpRequest &req) {
  try {
    const std::string user_id = getUserIdFromToken(req);
    std::string target = req.target();
    size_t query_pos = target.find('?');
    if (query_pos != std::string::npos) {
      target = target.substr(0, query_pos);
    }

    for (const auto &route : routes_) {
      if (route.method == req.method()) {
        if (target == route.pattern) {
          return route.handler(req, user_id);
        }
      }
    }
    return not_found_handler_(req);

  } catch (const std::exception &e) {
    std::cout << "Error trying to handle request: " << e.what() << std::endl;
    return error_handler_(req);
  }
}

HttpResponse Router::createResponse(http::status status,
                                    std::string_view body) {
  HttpResponse res{status, 11}; // HTTP/1.1
  res.set(http::field::server, "HTTP Server");
  res.set(http::field::content_type, "application/json");
  res.body() = body;
  res.prepare_payload();
  return res;
}
