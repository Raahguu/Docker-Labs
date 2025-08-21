#include <string>
#include <vector>
#include <curl/curl.h>

class Curl_Wrapper {
public:
	Curl_Wrapper();
	~Curl_Wrapper();
	std::string Get(const std::string& url, const std::vector<std::string>& headers);
	std::string Post();
	std::string Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers);
private:
	CURL* curl;
	CURLcode res;
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

};