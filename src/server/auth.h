#include "curl/curl.h"
#include "nlohmann/json.hpp"
#include "router.h"
#include <string_view>

using json = nlohmann::json;

std::string getUserIdFromToken(const HttpRequest &req);
