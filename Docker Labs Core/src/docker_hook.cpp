#include <iostream>
#include "docker_labs/core/docker_hook.h"
#include "docker_labs/core/curl_wrapper.h"


using namespace Docker_Labs;

Labs_Core::Docker::Docker()
{
	this->curl = Labs_Core::Curl_Wrapper();
}

json Labs_Core::Docker::CallDockerAPI(const std::string& path, const std::string& data, std::string method)
{
	std::string url = "http://localhost/v1.51" + path;

	std::vector<std::string> headers = {
		"Content-Type: application/json"
	};

	long httpCode;
	std::string response;
	
	#ifdef _WIN32
		std::string socket = "npipe:////./pipe/docker_engine";
	#else
		std::string socket = "/var/run/docker.sock";
	#endif

	std::tie(httpCode, response) = curl.Socket_Request(url, data, headers, method, socket);

	json result;

	try {
		result["httpCode"] = httpCode;
	} catch (...){
		std::cerr << "Assigning httpCode throws an error" << std::endl;
		std::cerr << "httpCode is '" << httpCode << "'" << std::endl;
		std::cerr << "the curl response was '" << response << std::endl;
		throw "Error";
	}

	// Try to parse the response as JSON
	try {
		if (response != "") {result["body"] = json::parse(response);}
	}
	catch (json::parse_error& e) {
		// If the response isn't JSON, return it as a plain string
		std::cerr << "json parsing threw an error" << std::endl;
		std::cerr << "url: " << path << std::endl;
		std::cerr << e.what() << std::endl;
		result["body"] = response;
		std::cerr << response;
	}

	return result;
}

json Labs_Core::Docker::CallDockerAPI(const std::string& path){
	return CallDockerAPI(path, "", "GET");
}

int Labs_Core::Docker::Test_API()
{
	std::string url = "/info";
	json response = CallDockerAPI(url);

	return response["httpCode"];
}




