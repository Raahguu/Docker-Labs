#include <iostream>
#include <tuple>
#include "curl_wrapper.h"

using namespace Docker_Labs;

Labs_Core::Curl_Wrapper::Curl_Wrapper()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
}

std::size_t Labs_Core::Curl_Wrapper::WriteCallback(char* contents, std::size_t size, std::size_t nmemb, std::string* userp) {
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), size * nmemb);
	return size * nmemb;
}


std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Socket_Request(const std::string& url, const std::string& data, const std::vector<std::string>& headers, std::string& request_type, std::string socket)
{
	curl_easy_reset(curl);

	if (curl) {
		long return_code;
		std::string responce;
		struct curl_slist* chunk = NULL;

		for (const std::string& header : headers) {
			chunk = curl_slist_append(chunk, header.c_str());
		}
		if(socket != ""){
			curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, socket.c_str());

		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request_type.c_str());
		
		if(request_type == "POST" || request_type == "PUT" || request_type == "PATCH"){
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		}

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responce);
		curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

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

std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Web_Request(const std::string& url, const std::string& data, const std::vector<std::string>& headers, std::string& request_type)
{
	return Labs_Core::Curl_Wrapper::Socket_Request(url, data, headers, request_type, "");
}

std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Web_Request(const std::string& url, const std::vector<std::string>& headers, std::string& request_type)
{
	return Labs_Core::Curl_Wrapper::Socket_Request(url, (std::string)"", headers, request_type, "");
}

std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Get(const std::string& url, std::vector<std::string>& headers) { std::string request_type = "GET"; return Web_Request(url, headers, request_type); }
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Delete(const std::string& url, std::vector<std::string>& headers) { std::string request_type = "DELETE"; return Web_Request(url, headers, request_type); }
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers) { std::string request_type = "POST"; return Web_Request(url, data, headers, request_type); }
std::tuple<long, std::string> Labs_Core::Curl_Wrapper::Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers) { std::string request_type = "PUT"; return Web_Request(url, data, headers, request_type); }
