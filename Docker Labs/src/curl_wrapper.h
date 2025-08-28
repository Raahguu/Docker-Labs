#pragma once
#include <string>
#include <vector>
#include <curl/curl.h>

namespace Docker_Labs {
	class Curl_Wrapper {
	public:
		Curl_Wrapper();
		~Curl_Wrapper();
		std::tuple<long, std::string> Get(const std::string& url, std::vector<std::string>& headers);
		std::tuple<long, std::string> Delete(const std::string& url, std::vector<std::string>& headers);
		std::tuple<long, std::string> Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers);
		std::tuple<long, std::string> Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers);
		static std::size_t WriteCallback(char* contents, std::size_t size, std::size_t nmemb, std::string* userp);
		CURL* curl;
		CURLcode res;
		std::tuple<long, std::string> Socket_Request(const std::string& url, const std::string& data, const std::vector<std::string>& headers, std::string& request_type, std::string socket);
	private:
		std::tuple<long, std::string> Web_Request(const std::string& url, const std::string& data, const std::vector<std::string>& headers, std::string& request_type);
		std::tuple<long, std::string> Web_Request(const std::string& url, const std::vector<std::string>& headers, std::string& request_type);

	};
}