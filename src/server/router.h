#pragma once

#include <boost/beast/http.hpp>
#include <functional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace http = boost::beast::http;

using HttpRequest = http::request<http::string_body>;
using HttpResponse = http::response<http::string_body>;
using RouteHandler =
    std::function<HttpResponse(const HttpRequest &, std::string_view)>;
using NonIdRouteHandler = std::function<HttpResponse(const HttpRequest &)>;

class Router {
private:
  struct Route {
    std::string pattern;
    RouteHandler handler;
    http::verb method;
  };

  std::vector<Route> routes_;
  NonIdRouteHandler not_found_handler_;
  NonIdRouteHandler error_handler_;

public:
  Router();

  void get(const std::string &pattern, RouteHandler handler);
  void post(const std::string &pattern, RouteHandler handler);
  void put(const std::string &pattern, RouteHandler handler);
  void delete_(const std::string &pattern, RouteHandler handler);

  void setNotFoundHandler(RouteHandler handler);
  void setErrorHandler(RouteHandler handler);

  HttpResponse handleRequest(const HttpRequest &req);

private:
  void addRoute(http::verb method, const std::string &pattern,
                RouteHandler handler);
  HttpResponse createResponse(http::status, std::string_view body);
};
