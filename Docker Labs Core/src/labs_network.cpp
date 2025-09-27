#include <iostream>
#include "docker_labs/core/docker_hook.h"
#include "docker_labs/core/labs_network.h"
#include "docker_labs/core/labs_container.h"

using namespace Docker_Labs;

Labs_Core::Network::Network(std::string id) : id(id) {
	Cache_Update();
};

//Accessors
std::string Labs_Core::Network::Get_ID() {
	return id;
};

//Cache Accessors
std::string Labs_Core::Network::Get_Name_Cache() {
	return name_cache;
};

std::string Labs_Core::Network::Get_Subnet_Cache() {
	return subnet_cache;
};
std::string Labs_Core::Network::Get_Gateway_Cache() {
	return gateway_cache;
};
std::vector<Labs_Core::Container> Labs_Core::Network::Get_Containers_Cache() {
	return containers_cache;
};

std::string Labs_Core::Network::Get_IP_Range_Cache() {
	return IPRange;
}

int Labs_Core::Network::Cache_Update() {
	Labs_Core::Docker docker = Labs_Core::Docker();
	name_cache = docker.Get_Network_Name(*this);
	subnet_cache = docker.Get_Subnet(*this);
	gateway_cache = docker.Get_Gateway(*this);
	containers_cache = docker.Get_Containers(*this);
	try {
		IPRange = docker.Get_IP_Range(*this);
	} catch (...) {
		std::cerr << "Updating network " << name_cache << "'s IPRange cache throws an error" << std::endl;
		throw;
	}
	return 0;
};
