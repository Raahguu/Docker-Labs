#include "labs_container.h"
#include "main.h"
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
    long httpCode = 0;

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
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        } else {
            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    json result;
    result["httpCode"] = httpCode;

    // Try to parse the response as JSON
    try {
        result["body"] = json::parse(response);
    } catch (json::parse_error& e) {
        // If the response isn't JSON, return it as a plain string
        result["body"] = response;
    }

    return result;
}

uint64_t str_to_long(const std::string& str) {
    uint64_t result = 0;
    for (size_t i = 0; i < str.size(); i++) {
        result |= static_cast<uint64_t>(static_cast<uint8_t>(str[i])) << (8 * i);
    }
    return result + 1000000;
}

std::string long_to_str(uint64_t value, size_t length) {
    std::string result;
	value -= 1000000;
    for (size_t i = 0; i < length; i++) {
        char c = static_cast<char>((value >> (8 * i)) & 0xFF);
        result.push_back(c);
    }
    return result;
}

namespace Docker_Labs {	
	//Constructors

	Container::Container(std::string id) : id(id) {}

	Container::Container(std::string email, std::string image_name, std::string container_name) {
		std::string url = "/containers/create?name=" + container_name;

		std::string request_data = R"({
			"Image": ")" + image_name + R"(",
			"HostConfig": {
				"PidsLimit": )" + str_to_long(email) + R"(
			}
		})";

		json response = CallDockerAPI(url, request_data, "POST");

		int httpCode = response["httpCode"]

		if(httpCode != 201){
			std::cout << "Error " << httpCode << ": " << response["body"]["message"] << std::endl;
			return httpCode;
		}
		std::cout << "Successfully created " << container_name << std::endl;

		this->id = response["body"]["Id"];
	}
	
	Container::Container(User user, std::string image_name, std::string container_name) : Container(user.Get_Email(), image_name, container_name) {}
	
	Container::Container::Container(std::string email, std::string image_name) : Container(email, image_name, email) {}
	
	Container::Container(User user, std::string image_name) : Container(user.Get_Email(), image_name) {}
	
	//Gets
	std::string Container::Get_ID(){ 
		return this->id;
	}
	
	std::string Container::Get_Name(){
		std::string url = "/containers/" + this->id + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting name of container " << this->id << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["Names"].empty() ? this->id : response["body"]["Names"][0].substr(1);
	}
	
	std::string Container::Get_IP(){
		std::string url = "/containers/" + this->id + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting IP of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["NetworkSettings"]["Networks"]["bridge"]["IPAddress"];
	}

	
	std::string Container::Get_Image(){
		std::string url = "/containers/" + this->id + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting image of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["Config"]["Image"];
	}
	

	std::string Container::Get_Owner(){
		std::string url = "/containers/" + this->Get_Name() + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting owner of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return long_to_str(response["body"]["HostConfig"]["PidsLimit"]);
	}
	
	std::vector<std::string> Container::Get_Networks(){
		std::string url = "/containers/" + this->name + "/json";
		json response = CallDockerAPI(url);
	
		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting image of container " << this->name << ": " << response["body"]["message"] << std::endl;
			return "";
		}
			
		std::vector<std::string> networks;
	
		for(json& network : response["body"]["NetworkSettings"]["Networks"]){
			networks.push_back(network["NetworkID"]);
		}
		
		return networks;
	}
	
	
	bool Container::Get_Status(){
		std::string url = "/containers/" + this->id + "/json";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting status of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["state"]["running"];
	}
	
	
	//Controllers
	int Container::Start(){
		std::string url = "/containers/" + container.Get_ID() + "/start";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " starting container " << container.Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 304) {
			std::cout << "Container " << container.Get_Name() << " was already up" << std::endl;
			return 304;
		}
		
		std::cout << "Container " << container.Get_Name() << " successfully started" << std::endl;

		return 204;
	}

	int Container::Stop(){
		std::string url = "/containers/" + this->id + "/stop";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " stopping container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 304) {
			std::cout << "Container " << this->Get_Name() << " was already down" << std::endl;
			return 304
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully stopped" << std::endl;

		return 204;
	}
	
	
	int Container::Restart(){
		std::string url = "/containers/" + this->id + "/restart";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " restarting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully restarted" << std::endl;

		return 204;
	}
	
	int Container::Kill(){
		std::string url = "/containers/" + this->id + "/kill";
		json response = CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " killing container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 409) {
			std::cout << "Container " << this->Get_Name() << " was already stopped" << std::endl;
			return 409
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully stopped" << std::endl;

		return 204;
	}
	
	
	int Container::Remove(){
		std::string url = "/containers/" + this->id;
		json response = CallDockerAPI(url, "", "DELETE");

		if(response["httpCode"] == 400 || response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " deleting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 409) {
			std::cout << "Container " << this->Get_Name() << " was running and so cannot be removed" << std::endl;
			return 409
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully removed" << std::endl;

		return 204;
	}
	
	int Container::Set_Owner(std::string email){
		std::string url = "/containers/" + this->id + "/update";
		std::string data = R"({"PidsLimit": )" + str_to_long(email) + "}";
		json response = CallDockerAPI(url, data, "POST");
		
		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " setting the owner of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		
		std::cout << "Successfully changed the owner of " << this->Get_Name() << " to be " << email << std::endl;
		return 200
	}
	
	int Container::Set_Owner(User user){
		return Set_Owner(user.email);
	}
}


Container Docker_Labs::Docker::Get_Container(std::string container_name){
		Container container;
	
		std::string url = "/containers/json?all=true&filters=%7B%22name%22%3A%5B%22^" + container_name + "$%22%5D%7D";
		json response = CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting container " << container_name << ": " << response["body"]["message"] << std::endl;
			return container;
		}
		
		return Container(response["body"][0]["Id"]);
}

int Docker_Labs::Docker::Kill(User user){
	std::vector<Container> containers = Get_Owned_Containers(user);

	int final_response = 204;

	for(Container& container : containers){
		int response = container.Kill();
		if(response != 204){
			final_response = response
		}
	}
	
	return final_response;
}

int Docker_Labs::Docker::Remove(User user){
	std::vector<Container> containers = Get_Owned_Containers(user);

	int final_response = 204;

	for(Container& container : containers){
		int response = container.Remove();
		if(response != 204){
			final_response = response;
		}
	}
	
	return final_response;
}

int Docker_Labs::Docker::Restart(User user){
	std::vector<Container> containers = Get_Owned_Containers(user);

	int final_response = 204;

	for(Container& container : containers){
		int response = container.Restart();
		if(response != 204){
			final_response = response;
		}
	}
	
	return final_response;
}


int Docker_Labs::Docker::Stop(User user){
	std::vector<Container> containers = Get_Owned_Containers(user);

	int final_response = 204;

	for(Container& container : containers){
		int response = container.Stop();
		if(response != 204 && response != 304){
			final_response = response;
		}
	}
	
	return final_response;
}

int Docker_Labs::Docker::Start(User user){
	std::vector<Container> containers = Get_Owned_Containers(user);

	int final_response = 204;

	for(Container& container : containers){
		int response = container.Start();
		if(response != 204 && response != 304){
			final_response = response;
		}
	}
	
	return final_response;
}

std::vector<Container> Docker_Labs::Docker::Get_Owned_Containers(std::string email){
	std::string url = "/containers/json?all=true";
	json response = CallDockerAPI(url);
	
	if(response["httpCode"] != 200){
		std::cout << "Error " << response["httpCode"] << " getting owned containers: " << reponse["body"]["message"] << std::endl;
		return std::vector<Container>();
	}

	std::vector<Container> owned_Containers;

	for(const auto& container_json : response["body"]){
		std::string container_id = container_json["Id"];

		if(Get_Owner(Container(container_id)) == email){
			Container container = Container(container_id);
			owned_Containers.push_back(container);
		}
	}

	std::cout << "Successfuly got all owned containers of " << user.Get_Email();
	
	return owned_Containers;
}

std::vector<Container> Docker_Labs::Docker::Get_Owned_Containers(User user){
	return Get_Owned_Containers(user.Get_Email());
}
