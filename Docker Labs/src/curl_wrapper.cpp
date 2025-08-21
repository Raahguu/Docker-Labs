#include "curl_wrapper.h"
#include <iostream>

Curl_Wrapper::Curl_Wrapper()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
}

Curl_Wrapper::~Curl_Wrapper()
{
	curl_easy_cleanup(curl);
}

size_t Curl_Wrapper::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), size * nmemb);
	return size * nmemb;
}


std::string Curl_Wrapper::Get(const std::string& url, const std::vector<std::string>& headers)
{
	if (curl) {

		std::string url = url;
		std::vector<std::string> headers = headers;
		
		struct curl_slist* chunk = NULL;
		std::string responce;

		for (const std::string& header : headers) {
			chunk = curl_slist_append(chunk, header.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responce);
		curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";
		}

		curl_slist_free_all(chunk);
		curl_easy_reset(curl);

		if (res != CURLE_OK) {
			throw "a tantrum";
		}

		return responce;
	}
	return std::string();
}

std::string Curl_Wrapper::Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers)
{
	if (curl) {

		std::string data = "data";
		std::cout << "Body size: " << data.size();


		struct curl_slist* chunk = NULL;
		std::string responce;


		for (const std::string& header : headers) {
			chunk = curl_slist_append(chunk, header.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size()-1);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responce);
		curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";
		}

		curl_slist_free_all(chunk);
		curl_easy_reset(curl);

		if (res != CURLE_OK) {
			throw "a tantrum";
		}

		return responce;
	}
	return std::string();
}

std::string Curl_Wrapper::Post()
{
	return std::string();
}
