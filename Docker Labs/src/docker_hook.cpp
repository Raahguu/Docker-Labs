#include "main.h"
#include "docker_hook.h"
#include <iostream>
#include <string>
//#include <curl/curl.h>

using namespace std;

//std::string PerformDockerPost(const std::string& url, const std::string& data = "", const std::string& method = "GET") {
//    CURL* curl;
//    CURLcode res;
//    std::string response;
//
//    curl_global_init(CURL_GLOBAL_DEFAULT);
//    curl = curl_easy_init();
//
//    if (curl) {
//        struct curl_slist* headers = NULL;
//        headers = curl_slist_append(headers, "Content-Type: application/json");
//
//        curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, "/var/run/docker.sock");
//        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//        
//		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
//
//		if(method == "POST" || method == "PUT" || method == "PATCH") {
//			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
//		}
//
//
//        // Capture response
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
//            +[](char* ptr, size_t size, size_t nmemb, std::string* userdata) -> size_t {
//                userdata->append(ptr, size * nmemb);
//                return size * nmemb;
//            }
//        );
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
//
//        res = curl_easy_perform(curl);
//        if (res != CURLE_OK) {
//            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
//        }
//
//        curl_slist_free_all(headers);
//        curl_easy_cleanup(curl);
//    }
//
//    curl_global_cleanup();
//    return response;
//}
//
//namespace Docker_Labs::Docker {
//    Container Create(User user, std::string image_name) {
//		std::string container_name = user.email;
//
//		std::string create_url = "http://localhost/containers/create?name=" + container_name;
//		std::string request_data = R"({
//		"Image": ")" + image_name + R"(",
//			"Labels": {
//				"owner": ")" + user.email + R"("
//			}
//		})";
//
//		std::string response_json = PerformDockerPost(create_url, request_data, "POST");
//
//		// (Optional) Debug
//		std::cout << "Docker API response: " << response_json << std::endl;
//
//		Container container;
//		container.name = container_name;
//		container.image = image_name;
//		return container;
//	}
//}
