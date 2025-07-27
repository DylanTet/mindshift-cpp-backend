#include "auth.h"
#include <iostream>

size_t write_data(void *ptr, size_t size, size_t nmemb, std::string *data) {
  data->append((char *)ptr, size * nmemb);
  return size * nmemb;
}

std::string getUserIdFromToken(const HttpRequest &req) {
  auto reqIter = req.find(http::field::authorization);
  if (reqIter == req.end()) {
    throw std::runtime_error("Missing user token in request");
  }

  std::string token = reqIter->value();
  long http_code;
  CURL *curl = curl_easy_init();
  if (curl) {
    const std::string CLERK_VERIFY_API =
        "https://api.clerk.com/v1/clients/verify";
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, &CLERK_VERIFY_API);

    std::string response_str;
    std::string header_str;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_str);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_str);

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers =
        curl_slist_append(headers, "Authorization: Bearer YOUR_SECRET_TOKEN");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    const std::string tokenObj = "{\"token\": \"" + token + "\"}";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, tokenObj.c_str());

    CURLcode ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
      throw std::runtime_error(curl_easy_strerror(ret));
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    curl = nullptr;

    json returned_json = json::parse(response_str);
    return returned_json["sessions"]["user_id"];
  } else {
    throw std::runtime_error("Error creating curl instance");
  }
}
