#include "docker_hook.h"
#include "labs_network.h"
#include "labs_container.h"

using namespace Docker_Labs;

Labs_Core::Network::Network(std::string id) : id(id) {};

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
int Labs_Core::Network::Cache_Update() {
	Labs_Core::Docker::Docker docker = Labs_Core::Docker::Docker();
	return 0;
};
