#pragma once

#include <boost/beast/http.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>

class DBClient;
class OpenAIClient;

namespace http = boost::beast::http;
using json = nlohmann::json;

class PostgresClient;
class OpenAIClient;

using HttpRequest = http::request<http::string_body>;
using HttpResponse = http::response<http::string_body>;

struct User {
  std::string id;
  std::string email;
  std::string firstName;
  std::string lastName;
};

class RequestHandler {
private:
  std::shared_ptr<DBClient> db_client_;
  std::shared_ptr<OpenAIClient> openai_client_;

public:
  RequestHandler(std::shared_ptr<DBClient> db_client,
                 std::shared_ptr<OpenAIClient> openai_client);

  HttpResponse handleHealth(const HttpRequest &req);

  HttpResponse handleChatCompletion(const HttpRequest &req);
  HttpResponse handleChatHistory(const HttpRequest &req);

  HttpResponse handleGetUser(const HttpRequest &req);
  HttpResponse handleCreateUser(const HttpRequest &req);
  HttpResponse handleUpdateUser(const HttpRequest &req);
  HttpResponse handleDeleteUser(const HttpRequest &req);

  HttpResponse handleGetUserJournals(const HttpRequest &req);
  HttpResponse handleCreateUserJournal(const HttpRequest &req);
  HttpResponse handleDeleteUserJournals(const HttpRequest &req);

private:
  HttpResponse createJsonResponse(http::status status, const json &data);
  HttpResponse createErrorResponse(http::status status,
                                   const std::string &message,
                                   const std::string &details = "");

  json parseRequestBody(const HttpRequest &req);

  bool validateJsonSchema(const json &data, const json &schema);
  std::string generateUUID();
};
