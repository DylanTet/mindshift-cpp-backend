#pragma once

#include "auth.h"
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

struct JournalData {
  std::string yesterdaysReflection;
  std::string inententions;
  std::string needEntry;
  std::string gratitudeEntry;
  uint8_t moodLevel;
  uint8_t energyLevel;
};

class RequestHandler {
private:
  std::shared_ptr<DBClient> db_client_;
  std::shared_ptr<OpenAIClient> openai_client_;

public:
  RequestHandler(std::shared_ptr<DBClient> db_client,
                 std::shared_ptr<OpenAIClient> openai_client);

  void runMiddleware(const HttpRequest &req);
  HttpResponse handleHealth(const HttpRequest &req);

  HttpResponse handleChatCompletion(const HttpRequest &req);
  HttpResponse handleChatHistory(const HttpRequest &req);

  HttpResponse handleGetUser(const HttpRequest &req, std::string_view user_id);
  HttpResponse handleCreateUser(const HttpRequest &req,
                                std::string_view user_id);
  HttpResponse handleUpdateUser(const HttpRequest &req,
                                std::string_view user_id);
  HttpResponse handleDeleteUser(const HttpRequest &req,
                                std::string_view user_id);

  HttpResponse handleGetUserJournals(const HttpRequest &req,
                                     std::string_view user_id);
  HttpResponse handleDeleteUserJournals(const HttpRequest &req,
                                        std::string_view user_id);
  HttpResponse handleAddUserJournal(const HttpRequest &req,
                                    std::string_view user_id);

private:
  HttpResponse createJsonResponse(http::status status, const json &data);
  HttpResponse createErrorResponse(http::status status,
                                   const std::string &message,
                                   const std::string &details = "");

  json parseRequestBody(const HttpRequest &req);
  std::string validateUser(const HttpRequest &req);
  bool validateJsonSchema(const json &data, const json &schema);
  std::string generateUUID();
};
