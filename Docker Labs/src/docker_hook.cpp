#include "main.h"
#include "docker_hook.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace std;

json CallDockerAPI(const std::string& url, const std::string& data = "", const std::string& method = "GET") {
    CURL* curl;
    CURLcode res;
    std::string response;
    long http_code = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::string FQDN = "http://localhost/v1.51" + url;

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, "/var/run/docker.sock");
        curl_easy_setopt(curl, CURLOPT_URL, FQDN.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());

        if (method == "POST" || method == "PUT" || method == "PATCH") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }

        // Capture response body
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
            +[](char* ptr, size_t size, size_t nmemb, std::string* userdata) -> size_t {
                userdata->append(ptr, size * nmemb);
                return size * nmemb;
            }
        );
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        } else {
            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    json result;
    result["http_code"] = http_code;

    // Try to parse the response as JSON
    try {
        result["body"] = json::parse(response);
    } catch (json::parse_error& e) {
        // If the response isn't JSON, return it as a plain string
        result["body"] = response;
    }

    return result;
}

uint64_t string_to_long(const std::string& str) {
    uint64_t result = 0;
    for (size_t i = 0; i < str.size(); i++) {
        result |= static_cast<uint64_t>(static_cast<uint8_t>(str[i])) << (8 * i);
    }
    return result + 1000000;
}

std::string long_to_string(uint64_t value, size_t length) {
    std::string result;
	value -= 1000000
    for (size_t i = 0; i < length; i++) {
        char c = static_cast<char>((value >> (8 * i)) & 0xFF);
        result.push_back(c);
    }
    return result;
}

namespace Docker_Labs::Docker {
	Container Create(std::string email, std::string image_name, std::string container_name) {
		Container container;
		container.name = container_name;
		container.image = image_name;

		std::string url = "/containers/create?name=" + container.name;

		std::string request_data = R"({
			"Image": ")" + container.image + R"(",
			"HostConfig": {
				"PidsLimit": )" + str_to_long(email) + R"(
			}
		})";

		json response = CallDockerAPI(url, request_data, "POST");

		int httpCode = response["http_code"]

		if(httpCode != 201){
			std::cout << "Error " << httpCode << ": " << response["body"]["message"] << std::endl;
			return httpCode;
		}
		std::cout << "Successfully created " << container.name << std::endl;

		container.id = response["body"]["Id"];
		container.ip = Get_IP(container);
		container.networks = Get_Networks(container.name);

		return container;
	}
	
	Container Create(User user, std::string image_name, std::string container_name) {
		return Create(user.email, image_name, container_name);
	}
	
	
	Container Create (std::string email, std::string image_name){
		//TODO: ask Laith how we are going to parse the email again
		std::string container_name = email;
		return Create(email, image_name, container_name);
	}
	
	Container Create (User user, std::string image_name){
		return Create(user, image_name);
	}
	

	int Start(Container container){
		std::string url = "/containers/" + container.id + "/start";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " starting container " << container.name << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 304) {
			std::cout << "Container " << container.name << " was already up" << std::endl;
			return 304
		}
		
		std::cout << "Container " << container.name << " successfully started" << std::endl;

		return 204;
	}
	

	std::vector<Container> Get_Owned_Containers(std::string email){
		std::string url = "/containers/json?all=true";
		json response = CallDockerAPI(url);
		
		if(response["httpCode"] != 200){
			std::cout << "Error " << response["httpCode"] << " getting owned containers: " << reponse["body"]["message"] << std::endl;
			return std::vector<Container>();
		}

		std::vector<Container> owned_Containers;

		for(const auto& container_json : response["body"]){
			std::string container_id = container_json["Id"];

			if(Get_Owner(container_id) == email){
				Container container;
				container.name = container_json["Names"].empty() ? "" : container_json["Names"][0].substr(1);
				container.image = container_json["Image"];
				container.id = container_json["Id"];
				container.ip = container_json["NetworkSettings"]["Networks"]["bridge"]["IPaddress"];
				container.networks = Get_Networks(container.name);
				owned_Containers.push_back(container);
			}
		}

		std::cout << "Successfuly got all owned containers of " << user.email;
		
		return owned_Containers;
	}
	
	std::vector<Container> Get_Owned_Containers(User user){
		return Get_Owned_Containers(user.email);
	}
	

	int Start(User user){
		std::vector<Container> containers = Get_Owned_Containers(user);

		int final_response = 204;

		for(Container& container : containers){
			int response = Start(container);
			if(response != 204 && response != 304){
				std::cout << "Error starting container " << container.name << ": " << response << std::endl;
				final_response = response;
			}
		}
		
		return final_response;
	}
	

	int Stop(Container container){
		std::string url = "/containers/" + container.id + "/stop";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " stopping container " << container.name << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 304) {
			std::cout << "Container " << container.name << " was already down" << std::endl;
			return 304
		}
		
		std::cout << "Container " << container.name << " successfully stopped" << std::endl;

		return 204;
	}
	
	int Stop(User user){
		std::vector<Container> containers = Get_Owned_Containers(user);

		int final_response = 204;

		for(Container& container : containers){
			int response = Stop(container);
			if(response != 204 && response != 304){
				final_response = response;
			}
		}
		
		return final_response;
	}
	
	
	int Restart(Container container){
		std::string url = "/containers/" + container.id + "/restart";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " restarting container " << container.name << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		
		std::cout << "Container " << container.name << " successfully restarted" << std::endl;

		return 204;
	}
	
	int Restart(User user){
		std::vector<Container> containers = Get_Owned_Containers(user);

		int final_response = 204;

		for(Container& container : containers){
			int response = Restart(container);
			if(response != 204){
				final_response = response;
			}
		}
		
		return final_response;
	}
	
	
	int Kill(Container container){
		std::string url = "/containers/" + container.id + "/kill";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " killing container " << container.name << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 409) {
			std::cout << "Container " << container.name << " was already stopped" << std::endl;
			return 409
		}
		
		std::cout << "Container " << container.name << " successfully stopped" << std::endl;

		return 204;
	}
	
	int Kill(User user){
		std::vector<Container> containers = Get_Owned_Containers(user);

		int final_response = 204;

		for(Container& container : containers){
			int response = Kill(container);
			if(response != 204){
				final_response = response
			}
		}
		
		return final_response;
	}
	
	
	int Remove(Container container){
		std::string url = "/containers/" + container.id;
		json response = CallDockerAPI(url, "", "DELETE");

		if(response["httpCode"] == 400 || response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " deleting container " << container.name << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 409) {
			std::cout << "Container " << container.name << " was running and so cannot be removed" << std::endl;
			return 409
		}
		
		std::cout << "Container " << container.name << " successfully removed" << std::endl;

		return 204;
	}
	
	int Remove(User user){
		std::vector<Container> containers = Get_Owned_Containers(user);

		int final_response = 204;

		for(Container& container : containers){
			int response = Remove(container);
			if(response != 204){
				final_response = response;
			}
		}
		
		return final_response;
	}
	
	
	std::string Get_IP(std::string container_name){
		std::string url = "/containers/" + container_name + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting IP of container " << container_name << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["NetworkSettings"]["Networks"]["bridge"]["IPAddress"];
	}
	
	std::string Get_IP(Container container){
		return Get_IP(container.name.empty() ? container.id : container.name);
	}

	
	std::string Get_Image(std::string container_name){
		std::string url = "/containers/" + container.id + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting image of container " << container.name << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["Config"]["Image"];
	}
	
	std::string Get_Image(Container container){
		return Get_Image(container.name.empty() ? container.id : container.name);
	}
	

	std::string Get_Owner(std::string container_name){
		std::string url = "/containers/" + container_name + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting owner of container " << container_name << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return long_to_str(response["body"]["HostConfig"]["PidsLimit"]);
	}
	
	std::string Get_Owner(Container container){
		return Get_Owner(container.name.empty() ? container.id : container.name);
	}
	
	
	
	int Set_Owner(std::string container_name, std::string email){
		std::string url = "/containers/" + container_name + "/update";
		std::string data = R"({"PidsLimit": )" + str_to_long(email) + "}";
		json response = CallDockerAPI(url, data, "POST");
		
		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " setting the owner of container " << container_name << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		
		std::cout << "Successfully changed the owner of " << container_name << " to be " << email << std::endl;
		return 200
	}
	
	int Set_Owner(Container container, User user){
		return Set_Owner(container.name.empty() ? container.id : container.name, user.email);
	}

	
	std::vector<std::string> Get_Networks(std::string container_name){
		std::string url = "/containers/" + container_name + "/json";
		json response = CallDockerAPI(url);
	
		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting image of container " << container_name << ": " << response["body"]["message"] << std::endl;
			return "";
		}
			
		std::vector<std::string> networks;
	
		for(json& network : response["body"]["NetworkSettings"]["Networks"]){
			networks.push_back(network["NetworkID"]);
		}
		
		return networks;
	}
	
	std::vector<std::string> Get_Network(Container container){
		return Get_Networks(container.name.empty() ? container.id : container.name);
	}

	Container Get_Container(std::string container_name){
		Container container;
	
		std::string url = "/containers/json?all=true&filters=%7B%22name%22%3A%5B%22^" + container_name + "$%22%5D%7D";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting container " << container_name << ": " << response["body"]["message"] << std::endl;
			return container;
		}
		
		container.id = response["body"][0]["Id"];
		container.name = response["body"][0]["Names"].empty() ? "" : response["body"][0]["Names"][0].substr(1);
		container.image = response["body"][0]["Image"];
		container.ip = response["body"][0]["NetworkSettings"]["Networks"]["bridge"]["IPAddress"];

		return container;
	}
	
	
	bool Get_Status(std::string container_name){
		std::string url = "/containers/" + container_name + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting status of container " << container_name << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["state"]["running"];
	}
	
	bool Get_Status(Container container){
		return Get_Status(container.name.empty() ? container.id : container.name);
	}
}
