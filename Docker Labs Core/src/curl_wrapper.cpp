#include <iostream>
#include <tuple>
#include "docker_labs/core/curl_wrapper.h"

using namespace Docker_Labs;

//
// ==== Constructor & Initialization ====
//

Labs_Core::Curl_Wrapper::Curl_Wrapper()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

//
// ==== Callback Function ====
//

// Callback for libcurl to write response data into a std::string
std::size_t Labs_Core::Curl_Wrapper::WriteCallback(char* contents, std::size_t size, std::size_t nmemb, std::string* userp) {
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

//
// ==== Core Request Handling ====
//

// Perform a cURL request, optionally using a UNIX socket
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Socket_Request(
    const std::string& url,
    const std::string& data,
    const std::vector<std::string>& headers,
    std::string& request_type,
    std::string socket)
{
    curl_easy_reset(curl);

    if (curl) {
        long return_code;
        std::string responce;
        struct curl_slist* chunk = NULL;

        // Set headers
        for (const std::string& header : headers) {
            chunk = curl_slist_append(chunk, header.c_str());
        }

        // Set UNIX socket path if provided
        if (socket != "") {
            curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, socket.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request_type.c_str());

        // Set payload for applicable methods
        if (request_type == "POST" || request_type == "PUT" || request_type == "PATCH") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }

        // Setup write callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responce);
        curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";
        }
        else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &return_code);
        }

        curl_slist_free_all(chunk);
        curl_easy_reset(curl);

        if (res != CURLE_OK) {
            throw "a tantrum";
        }

        return std::make_tuple(return_code, responce);
    }

    throw "a tantrum";
}

//
// ==== Web Request Wrappers ====
//

// Web request without UNIX socket
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Web_Request(
    const std::string& url,
    const std::string& data,
    const std::vector<std::string>& headers,
    std::string& request_type)
{
    return Labs_Core::Curl_Wrapper::Socket_Request(url, data, headers, request_type, "");
}

// Overloaded web request without payload data
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Web_Request(
    const std::string& url,
    const std::vector<std::string>& headers,
    std::string& request_type)
{
    return Labs_Core::Curl_Wrapper::Socket_Request(url, "", headers, request_type, "");
}

//
// ==== HTTP Method Convenience Functions ====
//

// HTTP GET
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Get(const std::string& url, std::vector<std::string>& headers)
{
    std::string request_type = "GET";
    return Web_Request(url, headers, request_type);
}

// HTTP DELETE
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Delete(const std::string& url, std::vector<std::string>& headers)
{
    std::string request_type = "DELETE";
    return Web_Request(url, headers, request_type);
}

// HTTP POST
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers)
{
    std::string request_type = "POST";
    return Web_Request(url, data, headers, request_type);
}

// HTTP PUT
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers)
{
    std::string request_type = "PUT";
    return Web_Request(url, data, headers, request_type);
}

// HTTP PATCH
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Patch(const std::string& url, const std::string& data, const std::vector<std::string>& headers)
{
    std::string request_type = "PATCH";
    return Web_Request(url, data, headers, request_type);
}
