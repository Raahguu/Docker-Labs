#include <iostream>
#include "docker_labs/core/docker_hook.h"
#include "docker_labs/core/curl_wrapper.h"

using namespace Docker_Labs;


std::vector<Labs_Core::Network> Labs_Core::Docker::Get_All_Networks() {
	std::string url = R"(/networks?filters={"label":["Docker-Labs=true"]})";
	json response = CallDockerAPI(url);

	if(response["httpCode"] == 500) {
		std::cerr << "Error " << response["httpCode"] << " getting networks: " <<  response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::vector<Labs_Core::Network> networks;

	for (json network : response["body"]) {
		networks.push_back(Network(network["Id"]));
	}

	return networks;
}

Labs_Core::Network Labs_Core::Docker::Create_Network(std::string network_name) {
	return Create_Network(network_name, false);
}

Labs_Core::Network Labs_Core::Docker::Create_Network(std::string network_name, bool internal_net){
	std::string url = "/networks/create";
	json response = CallDockerAPI(url, R"({"Name": ")" + network_name + R"(",)" 
		+ (internal_net ? R"("Internal": true, )" : "")
			+ R"("Labels": {
					"Docker-Labs": "true"
				}})", "POST");

	if (response["httpCode"] != 201) {
		std::cerr << "Error creating network " << network_name << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return Network(response["body"]["Id"]);
}

Labs_Core::Network Labs_Core::Docker::Create_Network(std::string network_name, std::string subnet, std::string gateway, std::string IP_Range, bool internal_net) {
	std::string url = "/networks/create";
	json response = CallDockerAPI(url, R"({"Name": ")" + network_name
		       	+ R"(", "IPAM": {"Config": [{"Subnet": ")" + subnet 
			+ R"(", "IPRange": ")" + IP_Range 
			+ R"(", "Gateway": ")" + gateway + R"("}],)" 
		+ (internal_net ? R"("Internal": true,)" : "")
			+ R"("Labels": {
					"Docker-Labs": "true"
				}} )", "POST");

	if (response["httpCode"] != 201) {
		std::cerr << "Error creating network " << network_name << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return Network(response["body"]["Id"]);
}

int Labs_Core::Docker::Delete_Network(Labs_Core::Network network) {
	std::string url = "/networks/" + network.Get_ID();
	json response = CallDockerAPI(url, "", "DELETE");
	
	if (response["httpCode"] != 204) {
		std::cerr << "Error deleting network " << network.Get_Name_Cache() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["httpCode"];
};


int Labs_Core::Docker::Add_To_Network(Labs_Core::Network network, Labs_Core::Container container) {
	std::string url = "/networks/" + network.Get_ID() + "/connect";
	json response = CallDockerAPI(url, R"({"Container": ")" + container.Get_ID() + R"("})", "POST");

	if (response["httpCode"] != 200) {
		std::cerr << "Error adding container " + container.Get_Name_Cache() + "to network " << network.Get_Name_Cache() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["httpCode"];
};

int Labs_Core::Docker::Remove_From_Network(Labs_Core::Network network, Labs_Core::Container container) {	
	std::string url = "/networks/" + network.Get_ID() + "/disconnect";
	json response = CallDockerAPI(url, R"({"Container": ")" + container.Get_ID() + R"(", "Force": true})", "POST");

	if (response["httpCode"] != 200) {
		std::cerr << "Error adding container " + container.Get_Name_Cache() + "to network " << network.Get_Name_Cache() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["httpCode"];
};


std::string Labs_Core::Docker::Get_Network_ID(Labs_Core::Network network) {
	return network.Get_ID();
};

std::string Labs_Core::Docker::Get_Network_Name(Labs_Core::Network network) {
	std::string url = "/networks/" + network.Get_ID();
	json response = CallDockerAPI(url);

	if (response["httpCode"] != 200){
		std::cerr << "Error getting name of network " << network.Get_ID() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["Name"];
};

std::string Labs_Core::Docker::Get_Subnet(Labs_Core::Network network) {
	std::string url = "/networks/" + network.Get_ID();
	json response = CallDockerAPI(url);

	if (response["httpCode"] != 200){
		std::cerr << "Error Getting Subnet of network " << network.Get_Name_Cache() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["IPAM"]["Config"][0]["Subnet"];
};

std::string Labs_Core::Docker::Get_Gateway(Labs_Core::Network network) {
	std::string url = "/networks/" + network.Get_ID();
	json response = CallDockerAPI(url);

	if (response["httpCode"] != 200){
		std::cerr << "Error Getting Gateway of network " << network.Get_Name_Cache() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return response["body"]["IPAM"]["Config"][0]["Gateway"];
};

std::string Labs_Core::Docker::Get_IP_Range(Labs_Core::Network network) {
	std::string url = "/networks/" + network.Get_ID();
	json response = CallDockerAPI(url);

	if (response["httpCode"] != 200){
		std::cerr << "Error Getting IP range of network " << network.Get_Name_Cache() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	try {
		return response["body"]["IPAM"]["Config"][0]["IPRange"];
	} catch (const nlohmann::json::type_error&) {
		return response["body"]["IPAM"]["Config"][0]["Subnet"];
	}
};

std::vector<Labs_Core::Container> Labs_Core::Docker::Get_Containers(Labs_Core::Network network) {
	std::string url = "/networks/" + network.Get_ID();
	json response = CallDockerAPI(url);

	if (response["httpCode"] != 200){
		std::cerr << "Error Getting Subnet of network " << network.Get_Name_Cache() << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	std::map<std::string, std::map<std::string, std::string>> containers_map = response["body"]["Containers"].get<std::map<std::string, std::map<std::string, std::string>>>();

	std::vector<Labs_Core::Container> containers;

	for (std::map<std::string, std::map<std::string, std::string>>::iterator it = containers_map.begin();
			it != containers_map.end(); ++it) {
		containers.push_back(Container(it->first));
	}

	return containers;
};

Labs_Core::Network Labs_Core::Docker::Get_Network(std::string network_name) {
	std::string url = "/networks/" + network_name;
	json response = CallDockerAPI(url);

	if (response["httpCode"] != 200) {
		std::cerr << "Error Getting network " << network_name << ": " << response["httpCode"] << " " << response["body"]["message"] << std::endl;
		throw "Error";
	}

	return Network(response["body"]["Id"]);
}
