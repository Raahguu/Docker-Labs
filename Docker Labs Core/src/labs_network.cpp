#include "docker_hook.h"
#include "labs_network.h"
#include "labs_container.h"

Docker_Labs::Network::Network(std::string id) : id(id) {};

//Accessors
std::string Docker_Labs::Network::Get_ID() {
	return id;
};

//Cache Accessors
std::string Docker_Labs::Network::Get_Name_Cache() {
	return name_cache;
};

std::string Docker_Labs::Network::Get_Subnet_Cache() {
	return subnet_cache;
};
std::string Docker_Labs::Network::Get_Gateway_Cache() {
	return gateway_cache;
};
std::vector<Docker_Labs::Container> Docker_Labs::Network::Get_Containers_Cache() {
	return containers_cache;
};
int Docker_Labs::Network::Cache_Update() {
	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	return 0;
};
