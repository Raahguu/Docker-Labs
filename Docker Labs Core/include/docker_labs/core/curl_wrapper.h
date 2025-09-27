// ---------------------------------------------
// @file curl_wrapper.h
// @brief Declares a thin wrapper around libcurl for HTTP and socket-based web requests.
//
// This class provides convenience methods for executing HTTP requests (GET, POST, PUT, etc.)
// with customizable headers and payloads, and includes support for UNIX socket requests.
//
// Dependencies:
// - <curl/curl.h>
// - <string>
// - <vector>
//
// Namespace:
// - Docker_Labs::Labs_Core
// ---------------------------------------------

#pragma once
#include <curl/curl.h>
#include <string>
#include <vector>

namespace Docker_Labs::Labs_Core {

	class Curl_Wrapper {
	public:
		// ---------------------------------------------
		// Constructor & Setup
		// ---------------------------------------------
		Curl_Wrapper();

		// ---------------------------------------------
		// Standard HTTP Methods
		// ---------------------------------------------
		std::tuple<long, std::string> Get(const std::string& url, std::vector<std::string>& headers);
		std::tuple<long, std::string> Delete(const std::string& url, std::vector<std::string>& headers);
		std::tuple<long, std::string> Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers);
		std::tuple<long, std::string> Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers);
		std::tuple<long, std::string> Patch(const std::string& url, const std::string& data, const std::vector<std::string>& headers);

		// ---------------------------------------------
		// UNIX Socket Request
		// ---------------------------------------------
		std::tuple<long, std::string> Socket_Request(
			const std::string& url,
			const std::string& data,
			const std::vector<std::string>& headers,
			std::string& request_type,
			std::string socket
		);

		// ---------------------------------------------
		// Utility: libcurl write callback
		// ---------------------------------------------
		static std::size_t WriteCallback(char* contents, std::size_t size, std::size_t nmemb, std::string* userp);

		// ---------------------------------------------
		// Raw libcurl handles (optional direct access)
		// ---------------------------------------------
		CURL* curl;
		CURLcode res;

	private:
		// ---------------------------------------------
		// Internal helpers for web requests
		// ---------------------------------------------
		std::tuple<long, std::string> Web_Request(
			const std::string& url,
			const std::string& data,
			const std::vector<std::string>& headers,
			std::string& request_type
		);

		std::tuple<long, std::string> Web_Request(
			const std::string& url,
			const std::vector<std::string>& headers,
			std::string& request_type
		);
	};

} // namespace Docker_Labs::Labs_Core
