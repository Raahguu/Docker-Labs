#include "labs_container.h"
#include "main.h"
#include "curl_wrapper.h"
#include "docker_hook.h"
#include <iostream>
#include <string>
#include <curl/curl.h>


//json Docker_Labs::Container::CallDockerAPI(const std::string& path, const std::string& data = "", std::string method = "GET") {
//	std::string url = "http://localhost/v1.51" + path;
//
//	std::vector<std::string> headers = {
//		"Content-Type: application/json"
//	};
//
//	long httpCode;
//	std::string response;
//
//	std::tie(httpCode, response) = curl.Socket_Request(url, data, headers, method, (std::string)"/var/run/docker.sock");
//
//
//    json result;
//    result["httpCode"] = httpCode;
//
//    // Try to parse the response as JSON
//    try {
//        result["body"] = json::parse(response);
//    } catch (json::parse_error& e) {
//        // If the response isn't JSON, return it as a plain string
//        result["body"] = response;
//    }
//
//    return result;
//}

namespace Docker_Labs {	
	//Constructors
	//*
	Container::Container(std::string id) : id(id) {
		//this->curl = Docker_Labs::Curl_Wrapper();
		Cache_Update();
	}

	/*Container::Container(std::string container_name, std::string image_name) {
		this->curl = Docker_Labs::Curl_Wrapper();
		std::string url = "/containers/create?name=" + container_name;

		std::string request_data = R"({
			"Image": ")" + image_name + R"(",
			"Labels": {
				"Docker_Labs": "true"
			}
		})";

		json response = Docker_Labs::Container::CallDockerAPI(url, request_data, "POST");

		int httpCode = response["httpCode"];

		if(httpCode != 201){
			std::string error_message = "Error " + std::to_string(httpCode) + ": " + response["body"]["message"].dump();
			std::cout << error_message << std::endl;
			throw error_message;
		}
		std::cout << "Successfully created " << container_name << std::endl;

		this->id = response["body"]["Id"];
	}*/
	
	////Gets
	std::string Container::Get_ID(){ 
		return this->id;
	}
	////*
	//std::string Container::Get_Name(){
	//	std::string url = "/containers/" + this->id + "/json";
	//	json response = Docker_Labs::Container::CallDockerAPI(url);

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cout << "Error " << response["httpCode"] << " getting name of container " << this->id << ": " << response["body"]["message"] << std::endl;
	//		return "";
	//	}

	//	return response["body"]["Name"].empty() ? this->id : response["body"]["Name"].dump().substr(2, response["body"]["Name"].dump().length() - 3);
	//}
	////*
	//std::string Container::Get_IP(){
	//	std::string url = "/containers/" + this->id + "/json";
	//	json response = Docker_Labs::Container::CallDockerAPI(url);

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cerr << "Error " << response["httpCode"] << " getting IP of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		throw "Error";
	//	}

	//	return response["body"]["NetworkSettings"]["Networks"]["bridge"]["IPAddress"];
	//}
	////*
	//std::string Container::Get_Image(){
	//	std::string url = "/containers/" + this->id + "/json";
	//	json response = Docker_Labs::Container::CallDockerAPI(url);

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cerr << "Error " << response["httpCode"] << " getting image of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		throw "Error";
	//	}

	//	return response["body"]["Config"]["Image"];
	//}
	////*
	//std::vector<std::string> Container::Get_Networks(){
	//	std::string url = "/containers/" + this->Get_Name() + "/json";
	//	json response = Docker_Labs::Container::CallDockerAPI(url);
	//
	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cerr << "Error " << response["httpCode"] << " getting image of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		throw "Error";
	//	}
	//		
	//	std::vector<std::string> networks;
	//
	//	for(json& network : response["body"]["NetworkSettings"]["Networks"]){
	//		networks.push_back(network["NetworkID"]);
	//	}
	//	
	//	return networks;
	//}
	////*
	//bool Container::Get_Status(){
	//	std::string url = "/containers/" + this->id + "/json";
	//	json response = Docker_Labs::Container::CallDockerAPI(url);

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cerr << "Error " << response["httpCode"] << " getting status of container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		throw "Error";
	//	}

	//	return response["body"]["State"]["Running"];
	//}

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
		Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
		name_cache = docker.Get_Name(*this);
		image_cache = docker.Get_Image(*this);
		ip_cache = docker.Get_IP(*this);
		networks_cache = docker.Get_Networks(*this);
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
	
	
	////Controllers
	////*
	//int Container::Start(){
	//	std::string url = "/containers/" + this->Get_ID() + "/start";
	//	json response = Docker_Labs::Container::CallDockerAPI(url, "", "POST");

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cerr << "Error " << response["httpCode"] << " starting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		throw "Error";
	//	}
	//	if(response["httpCode"] == 304) {
	//		std::cout << "Container " << this->Get_Name() << " was already up" << std::endl;
	//		return 304;
	//	}
	//	
	//	std::cout << "Container " << this->Get_Name() << " successfully started" << std::endl;

	//	return 204;
	//}
	////*
	//int Container::Stop(){
	//	std::string url = "/containers/" + this->id + "/stop";
	//	json response = Docker_Labs::Container::CallDockerAPI(url, "", "POST");

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cerr << "Error " << response["httpCode"] << " stopping container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		throw "Error";
	//	}
	//	if(response["httpCode"] == 304) {
	//		std::cout << "Container " << this->Get_Name() << " was already down" << std::endl;
	//		return 304;
	//	}
	//	
	//	std::cout << "Container " << this->Get_Name() << " successfully stopped" << std::endl;

	//	return 204;
	//}
	////*
	//int Container::Restart(){
	//	std::string url = "/containers/" + this->id + "/restart";
	//	json response = Docker_Labs::Container::CallDockerAPI(url, "", "POST");

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cerr << "Error " << response["httpCode"] << " restarting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		throw "Error";
	//	}
	//	
	//	std::cout << "Container " << this->Get_Name() << " successfully restarted" << std::endl;

	//	return 204;
	//}
	////*
	//int Container::Kill(){
	//	std::string url = "/containers/" + this->id + "/kill";
	//	json response = Docker_Labs::Container::CallDockerAPI(url, "", "POST");

	//	if(response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cout << "Error " << response["httpCode"] << " killing container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		return response["httpCode"];
	//	}
	//	if(response["httpCode"] == 409) {
	//		std::cout << "Container " << this->Get_Name() << " was already stopped" << std::endl;
	//		return 409;
	//	}
	//	
	//	std::cout << "Container " << this->Get_Name() << " successfully stopped" << std::endl;

	//	return 204;
	//}
	////*	
	//int Container::Remove(){
	//	std::cout << "Removing Container " << this->Get_Name() << std::endl;
	//	std::string url = "/containers/" + this->id;
	//	json response = Docker_Labs::Container::CallDockerAPI(url, "", "DELETE");

	//	if(response["httpCode"] == 400 || response["httpCode"] == 404 || response["httpCode"] == 500){
	//		std::cout << "Error " << response["httpCode"] << " deleting container " << this->Get_Name() << ": " << response["body"]["message"] << std::endl;
	//		return response["httpCode"];
	//	}
	//	if(response["httpCode"] == 409) {
	//		std::cout << "Container " << this->Get_Name() << " was running and so cannot be removed" << std::endl;
	//		return 409;
	//	}
	//	
	//	std::cout << "Container successfully removed" << std::endl;

	//	return 204;
	//}
}

//Docker_Labs::Container Docker_Labs::Docker::Commands::Get_Container(std::string container_name) {	
//		std::string url = "/containers/json?all=true&filters=%7B%22name%22%3A%5B%22^" + container_name + "$%22%5D%7D";
//		json response = Docker_Labs::Container("").CallDockerAPI(url);
//
//		if(response["httpCode"] == 404 || response["httpCode"] == 500){
//			std::cerr << "Error " << response["httpCode"] << " getting container " << container_name << ": " << response["body"]["message"] << std::endl;
//			throw "Error";
//		}
//		
//		return Container(response["body"][0]["Id"]);
//}

//int Docker_Labs::Container::Test_API(){
//	std::string url = "/info";
//	json response = CallDockerAPI(url);
//	
//	return response["httpCode"];
//}
