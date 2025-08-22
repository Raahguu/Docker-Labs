#include "labs_container.h"
#include "main.h"
#include <iostream>
#include <string>
#include <curl/curl.h>

using namespace std;

json Docker_Labs::Docker::CallDockerAPI(const std::string& url, const std::string& data = "", const std::string& method = "GET") {
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

namespace Docker_Labs {	
	//Constructors

	Container::Container(std::string id) : id(id) {}

	Container::Container(std::string container_name, std::string image_name) {
		std::string url = "/containers/create?name=" + container_name;

		std::string request_data = R"({
			"Image": ")" + image_name + R"(",
			"Labels": {
				"Docker_Labs": "true"
			}
		})";

		json response = Docker_Labs::Docker::CallDockerAPI(url, request_data, "POST");

		int httpCode = response["httpCode"];

		if(httpCode != 201){
			std::string error_message = "Error " + std::to_string(httpCode) + ": " + response["body"]["message"].dump();
			std::cout << error_message << std::endl;
			throw error_message;
		}
		std::cout << "Successfully created " << container_name << std::endl;

		this->id = response["body"]["Id"];
	}
	
	//Gets
	std::string Container::Get_ID(){ 
		return this->id;
	}
	
	std::string Container::Get_Name(){
		std::string url = "/containers/" + this->id + "/json";
		json response = Docker_Labs::Docker::CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " getting name of container " << this->id << ": " << response["body"]["message"] << std::endl;
			return "";
		}

		return response["body"]["Name"].empty() ? this->id : response["body"]["Name"].dump().substr(2, response["body"]["Name"].dump().length() - 3);
	}
	
	std::string Container::Get_IP(){
		std::string url = "/containers/" + this->id + "/json";
		json response = Docker_Labs::Docker::CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " getting IP of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}

		return response["body"]["NetworkSettings"]["Networks"]["bridge"]["IPAddress"];
	}

	
	std::string Container::Get_Image(){
		std::string url = "/containers/" + this->id + "/json";
		json response = Docker_Labs::Docker::CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " getting image of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}

		return response["body"]["Config"]["Image"];
	}
	
	std::vector<std::string> Container::Get_Networks(){
		std::string url = "/containers/" + this->Get_Name() + "/json";
		json response = Docker_Labs::Docker::CallDockerAPI(url);
	
		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " getting image of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}
			
		std::vector<std::string> networks;
	
		for(json& network : response["body"]["NetworkSettings"]["Networks"]){
			networks.push_back(network["NetworkID"]);
		}
		
		return networks;
	}
	
	
	bool Container::Get_Status(){
		std::string url = "/containers/" + this->id + "/json";
		json response = Docker_Labs::Docker::CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " getting status of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}

		return response["body"]["State"]["Running"];
	}

	//Cache Gets

	std::string Container::Get_Name_Cache() {
		return name_cache;
	};
	std::string Container::Get_Image_Cache() {
		return image_cache;
	};
	std::string Container::Get_IP_Cache() {
		return ip_cache;
	};
	std::vector<std::string> Container::Get_Networks_Cache() {
		return networks_cache;
	};

	int Container::Cache_Update() {
		name_cache = Get_Name();
		image_cache = Get_Image();
		ip_cache = Get_IP();
		networks_cache = Get_Networks();
		return 0;
	}

	Container Container::Bogus(std::string id, std::string name, std::string image, std::string ip, std::vector<std::string> networks)
	{
		Container bogus_container = Container(id);
		bogus_container.name_cache = name;
		bogus_container.image_cache = image;
		bogus_container.ip_cache = ip;
		bogus_container.networks_cache = networks;
		return bogus_container;
	}
	
	
	//Controllers
	int Container::Start(){
		std::string url = "/containers/" + this->Get_ID() + "/start";
		json response = Docker_Labs::Docker::CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " starting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}
		if(response["httpCode"] == 304) {
			std::cout << "Container " << this->Get_Name() << " was already up" << std::endl;
			return 304;
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully started" << std::endl;

		return 204;
	}

	int Container::Stop(){
		std::string url = "/containers/" + this->id + "/stop";
		json response = Docker_Labs::Docker::CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " stopping container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}
		if(response["httpCode"] == 304) {
			std::cout << "Container " << this->Get_Name() << " was already down" << std::endl;
			return 304;
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully stopped" << std::endl;

		return 204;
	}
	
	
	int Container::Restart(){
		std::string url = "/containers/" + this->id + "/restart";
		json response = Docker_Labs::Docker::CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " restarting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully restarted" << std::endl;

		return 204;
	}
	
	int Container::Kill(){
		std::string url = "/containers/" + this->id + "/kill";
		json response = Docker_Labs::Docker::CallDockerAPI(url, "", "POST");

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " killing container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 409) {
			std::cout << "Container " << this->Get_Name() << " was already stopped" << std::endl;
			return 409;
		}
		
		std::cout << "Container " << this->Get_Name() << " successfully stopped" << std::endl;

		return 204;
	}
	
	
	int Container::Remove(){
		std::cout << "Removing Container " << this->Get_Name() << std::endl;
		std::string url = "/containers/" + this->id;
		json response = Docker_Labs::Docker::CallDockerAPI(url, "", "DELETE");

		if(response["httpCode"] == 400 || response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cout << "Error " << response["httpCode"] << " deleting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
			return response["httpCode"];
		}
		if(response["httpCode"] == 409) {
			std::cout << "Container " << this->Get_Name() << " was running and so cannot be removed" << std::endl;
			return 409;
		}
		
		std::cout << "Container successfully removed" << std::endl;

		return 204;
	}
}


Docker_Labs::Container Docker_Labs::Docker::Get_Container(std::string container_name) {	
		std::string url = "/containers/json?all=true&filters=%7B%22name%22%3A%5B%22^" + container_name + "$%22%5D%7D";
		json response = Docker_Labs::Docker::CallDockerAPI(url);

		if(response["httpCode"] == 404 || response["httpCode"] == 500){
			std::cerr << "Error " << response["httpCode"] << " getting container " << container_name << ": " << response["body"]["message"] << std::endl;
			throw "Error";
		}
		
		return Container(response["body"][0]["Id"]);
}


int Docker_Labs::Docker::Test_API(){
	std::string url = "/info";
	json response = Docker_Labs::Docker::CallDockerAPI(url);
	
	return response["httpCode"];
}
