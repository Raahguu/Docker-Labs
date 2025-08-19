#include "main.h"
#inclued "docker_hook.h"
#include <iostream>
#include <string>
#include <curl/curl.h>

using namespace std;

namespace Docker_Labs::Docker {
    int Create(User user, std::string image_name) {
        CURL* curl;
        CURLcode res;
        int status = 1; // assume failure

        std::string create_url = "http://localhost/containers/create?name=" + user.email;

        std::string post_data = R"({"Image": )" + image_name + R"(})";

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, "/var/run/docker.sock");
            curl_easy_setopt(curl, CURLOPT_URL, create_url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Docker container create failed: " << curl_easy_strerror(res) << std::endl;
            }
            else {
                std::cout << "Container creation requested successfully." << std::endl;
                status = 0; // success
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
        return container;
    }

}
