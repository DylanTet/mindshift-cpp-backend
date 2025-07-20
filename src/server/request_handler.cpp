#include "request_handler.h"
#include <ctime>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

RequestHandler::RequestHandler(std::shared_ptr<DBClient> db_client,
                               std::shared_ptr<OpenAIClient> openai_client)
    : openai_client_(std::move(openai_client)),
      db_client_(std::move(db_client)) {}

HttpResponse RequestHandler::handleHealth(const HttpRequest &req) {
  // TODO: this is just filler and will actually need to check the status of the
  // services
  json res = {{"status", "healthy"},
              {"timestamp", std::time(nullptr)},
              {"services",
               {"database", db_client_ ? "connected" : "disconnected"},
               {"openai", openai_client_ ? "configued" : "not configured"}}};

  return createJsonResponse(http::status::ok, res);
}

HttpResponse RequestHandler::handleChatCompletion(const HttpRequest &req) {
  try {
    json body = parseRequestBody(req);

    if (!body.contains("message") || !body["message"].is_string()) {
      return createErrorResponse(http::status::bad_request,
                                 "Missing or invalid response from OpenAI");
    }

    std::string message = body["message"];
    // default value
    std::string model = body.value("model", "gpt-3.5-turbo");

    // Make call to OpenAI API
    // std::string openai_response = openai_client_->chatCompletion(message,
    // model);
    // return createJsonResponse(http::status::ok, openai_response);

  } catch (std::exception &e) {
    // Do I want to send the actual chat error back to the user?
    return createErrorResponse(http::status::internal_server_error,
                               "Failed to process chat", e.what());
  }
}

HttpResponse RequestHandler::handleGetUser(const HttpRequest &req) {
  json body = parseRequestBody(req);
  std::string userId = body["userId"];
  if (userId.empty()) {
    return createErrorResponse(http::status::bad_request, "Missing user ID");
  }

  std::stringstream query;
  query << "SELECT * FROM users WHERE id = " << userId;

  try {
    // auto result = db_client_->getUser(userId);

    // if (result.empty()) {
    //   return createErrorResponse(http::status::not_found, "User not found");
    // }

    // auto row = result[0];
    // json userData = {{"id", row["id"].as<std::string>()},
    //                  {"email", row["email"].as<std::string>()},
    //                  {"firstName", row["firstName"].as<std::string>()},
    //                  {"lastName", row["lastName"].as<std::string>()}};
    json userData;
    return createJsonResponse(http::status::ok, userData);

  } catch (std::exception &e) {
    return createErrorResponse(http::status::internal_server_error,
                               "Error trying to get user.", e.what());
  }
}

HttpResponse RequestHandler::handleCreateUser(const HttpRequest &req) {
  json body = parseRequestBody(req);
  std::string userId = body["userId"];
  if (userId.empty()) {
    return createErrorResponse(http::status::bad_request, "Missing user ID");
  }

  std::string email = body["email"];
  std::string firstName = body["firstName"];
  std::string lastName = body["lastName"];
  User userInfo{userId, email, firstName, lastName};

  std::stringstream query;
  query << "INSERT INTO users (id, firstName, lastName, email, created_at) "
        << "VALUES ('" << userId << "', '" << firstName << "', '" << lastName
        << "', '" << email << "', NOW()) ";
  try {
    // auto result = db_client_->createUser(userInfo);
    // if (result.empty()) {
    //   return createErrorResponse(http::status::internal_server_error,
    //                              "Failed to create user");
    // }

    return createJsonResponse(http::status::ok,
                              {"message", "User created successfully"});
  } catch (std::exception &e) {
    std::cout << "Error adding user to database: " << e.what() << '\n';
    return createErrorResponse(http::status::internal_server_error,
                               "Error creating user");
  }
}

HttpResponse RequestHandler::createJsonResponse(http::status status,
                                                const json &data) {
  HttpResponse res{status, 11};
  res.set(http::field::server, "Mindshift Server");
  res.set(http::field::content_type, "application/json");
  res.body() = data.dump();
  res.prepare_payload();
  return res;
}

HttpResponse RequestHandler::createErrorResponse(http::status status,
                                                 const std::string &message,
                                                 const std::string &details) {
  json error_data = {{"error", message}, {"status", static_cast<int>(status)}};

  if (!details.empty()) {
    error_data["details"] = details;
  }

  return createJsonResponse(status, error_data);
}

json RequestHandler::parseRequestBody(const HttpRequest &req) {
  try {
    return json::parse(req.body());
  } catch (const json::parse_error &e) {
    throw std::runtime_error("Invalid JSON in request body: " +
                             std::string(e.what()));
  }
}
