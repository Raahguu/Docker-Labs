#include "docker_hook.h"
#include <iostream>

Docker_Labs::Docker::Docker::Docker()
{
	this->curl = Docker_Labs::Curl_Wrapper();
}



Docker_Labs::Container Docker_Labs::Docker::Docker::Get_Container(std::string container_name)
{
	std::string url = "/containers/json?all=true&filters=%7B%22name%22%3A%5B%22^" + container_name + "$%22%5D%7D";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting container " << container_name << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	Docker_Labs::Container container = Docker_Labs::Container(response["body"][0]["Id"]);
	container.Cache_Update();
	return container;
}

std::string Docker_Labs::Docker::Docker::Get_ID(Docker_Labs::Container container)
{
	return container.Get_ID();
}

std::string Docker_Labs::Docker::Docker::Get_Name(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cout << "Error " << response["httpCode"] << " getting name of container " << Get_ID(container) << ": " << response["body"]["message"] << std::endl;
		return "";
	}

	return response["body"]["Name"].empty() ? Get_ID(container) : response["body"]["Name"].dump().substr(2, response["body"]["Name"].dump().length() - 3);
}

std::string Docker_Labs::Docker::Docker::Get_Image(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting image of container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["Config"]["Image"];
}

std::string Docker_Labs::Docker::Docker::Get_IP(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting IP of container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["NetworkSettings"]["Networks"]["bridge"]["IPAddress"];
}

std::vector<std::string> Docker_Labs::Docker::Docker::Get_Networks(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting image of container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::vector<std::string> networks;

	for (json& network : response["body"]["NetworkSettings"]["Networks"]) {
		networks.push_back(network["NetworkID"]);
	}

	return networks;
}

bool Docker_Labs::Docker::Docker::Get_Status(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/json";
	json response = CallDockerAPI(url);

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting status of container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["State"]["Running"];
}

Docker_Labs::Container Docker_Labs::Docker::Docker::Create_Container(std::string container_name, std::string image_name) {
	std::string url = "/containers/create?name=" + container_name;

	std::string request_data = R"({
			"Image": ")" + image_name + R"(",
			"Labels": {
				"Docker_Labs": "true"
			},
			"HostConfig": {
				"RestartPolicy": {
					"Name": "unless-stopped"
				}
			}
		})";

	json response = CallDockerAPI(url, request_data, "POST");

	int httpCode = response["httpCode"];

	if (httpCode != 201) {
		std::string error_message = "Error " + std::to_string(httpCode) + ": " + response["body"]["message"].dump();
		std::cout << error_message << std::endl;
		throw error_message;
	}
	std::cout << "Successfully created " << container_name << std::endl;

	Docker_Labs::Container container = Docker_Labs::Container(response["body"]["Id"]);
	container.Cache_Update();
	return container;
}

int Docker_Labs::Docker::Docker::Start(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/start";
	json response = CallDockerAPI(url, "", "POST");

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " starting container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}
	if (response["httpCode"] == 304) {
		std::cout << "Container " << Get_Name(container) << " was already up" << std::endl;
		return 304;
	}

	std::cout << "Container " << Get_Name(container) << " successfully started" << std::endl;

	return 204;
}

int Docker_Labs::Docker::Docker::Stop(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/stop";
	json response = CallDockerAPI(url, "", "POST");

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " stopping container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}
	if (response["httpCode"] == 304) {
		std::cout << "Container " << Get_Name(container) << " was already down" << std::endl;
		return 304;
	}

	std::cout << "Container " << Get_Name(container) << " successfully stopped" << std::endl;

	return 204;
}

int Docker_Labs::Docker::Docker::Restart(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/restart";
	json response = CallDockerAPI(url, "", "POST");

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " restarting container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::cout << "Container " << Get_Name(container) << " successfully restarted" << std::endl;

	return 204;
}

int Docker_Labs::Docker::Docker::Kill(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container) + "/kill";
	json response = CallDockerAPI(url, "", "POST");

	if (response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cout << "Error " << response["httpCode"] << " killing container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		return response["httpCode"];
	}
	if (response["httpCode"] == 409) {
		std::cout << "Container " << Get_Name(container) << " was already stopped" << std::endl;
		return 409;
	}

	std::cout << "Container " << Get_Name(container) << " successfully stopped" << std::endl;

	return 204;
}

int Docker_Labs::Docker::Docker::Remove(Docker_Labs::Container container)
{
	std::string url = "/containers/" + Get_ID(container);
	json response = CallDockerAPI(url, "", "DELETE");

	if (response["httpCode"] == 400 || response["httpCode"] == 404 || response["httpCode"] == 500) {
		std::cout << "Error " << response["httpCode"] << " deleting container " << Get_Name(container) << ": " << response["body"]["message"] << std::endl;
		return response["httpCode"];
	}
	if (response["httpCode"] == 409) {
		std::cout << "Container " << Get_Name(container) << " was running and so cannot be removed" << std::endl;
		return 409;
	}

	std::cout << "Container successfully removed" << std::endl;

	return 204;
}

json Docker_Labs::Docker::Docker::CallDockerAPI(const std::string& path, const std::string& data, std::string method)
{
	std::string url = "http://localhost/v1.51" + path;

	std::vector<std::string> headers = {
		"Content-Type: application/json"
	};

	long httpCode;
	std::string response;

	std::tie(httpCode, response) = curl.Socket_Request(url, data, headers, method, (std::string)"/var/run/docker.sock");


	json result;
	result["httpCode"] = httpCode;

	// Try to parse the response as JSON
	try {
		result["body"] = json::parse(response);
	}
	catch (json::parse_error& e) {
		// If the response isn't JSON, return it as a plain string
		result["body"] = response;
	}

	return result;
}

int Docker_Labs::Docker::Docker::Test_API()
{
	std::string url = "/info";
	json response = CallDockerAPI(url);

	return response["httpCode"];
}
