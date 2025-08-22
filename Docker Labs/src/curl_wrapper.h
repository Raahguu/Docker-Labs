#include <string>
#include <vector>
#include <curl/curl.h>

class Curl_Wrapper {
public:
	Curl_Wrapper();
	~Curl_Wrapper();
	std::string Get(const std::string& url, std::vector<std::string>& headers);
	std::string Delete(const std::string& url, std::vector<std::string>& headers);
	std::string Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers);
	std::string Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers);
	static std::size_t WriteCallback(char* contents, std::size_t size, std::size_t nmemb, std::string* userp);
	CURL* curl;
	CURLcode res;
private:
	std::string Get(const std::string& url, std::vector<std::string>& headers, std::string& request_type);
	std::string Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers, std::string& request_type);

};