#include "database/database_client.h"
#include "external_apis/openai_client.h"
#include "server/http_server.h"
#include "server/request_handler.h"
#include "server/router.h"
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>

int main(int argc, char *argv[]) {
  const uint16_t PORT = 8000;
  const uint8_t NUMBER_OF_THREADS = 4;

  try {
    std::shared_ptr<OpenAIClient> openai_client =
        std::make_shared<OpenAIClient>();
    std::shared_ptr<DBClient> db_client = std::make_shared<DBClient>();

    std::shared_ptr<Router> router = std::make_shared<Router>();
    std::shared_ptr<RequestHandler> handler =
        std::make_shared<RequestHandler>(db_client, openai_client);

    // User Endpoints
    router->get("/health",
                [&handler](const HttpRequest &req, std::string_view) {
                  return handler->handleHealth(req);
                });

    router->get("/api/get-user",
                [&handler](const HttpRequest &req, std::string_view user_id) {
                  return handler->handleGetUser(req, user_id);
                });

    router->post("/api/chat",
                 [&handler](const HttpRequest &req, std::string_view) {
                   return handler->handleChatCompletion(req);
                 });

    router->post("/api/new-user",
                 [&handler](const HttpRequest &req, std::string_view user_id) {
                   return handler->handleCreateUser(req, user_id);
                 });

    // Journal Endpoints
    router->post("/api/add-journal-entry",
                 [&handler](const HttpRequest &req, std::string_view user_id) {
                   return handler->handleAddUserJournal(req, user_id);
                 });

    Server server("0.0.0.0", PORT, NUMBER_OF_THREADS);
    server.set_router(router);
    server.start();

    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  } catch (std::exception &e) {
    std::cerr << "Error running server: " << e.what() << '\n';
    return -1;
  }

  return 0;
}
