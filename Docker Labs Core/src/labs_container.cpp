#include "labs_container.h"
#include "docker_hook.h"

using namespace Docker_Labs;
//Constructors
Labs_Core::Container::Container(std::string id) : id(id) {
}

////Gets
std::string Labs_Core::Container::Get_ID() {
	return id;
}

std::string Labs_Core::Container::Get_Name_Cache() {
	return name_cache;
};
std::string Labs_Core::Container::Get_Image_Cache() {
	return image_cache;
};
std::string Labs_Core::Container::Get_IP_Cache() {
	return ip_cache;
};
std::vector<std::string> Labs_Core::Container::Get_Networks_Cache() {
	return networks_cache;
};

int Labs_Core::Container::Cache_Update() {
	Labs_Core::Docker docker = Labs_Core::Docker();
	name_cache = docker.Get_Name(*this);
	image_cache = docker.Get_Image(*this);
	ip_cache = docker.Get_IP(*this);
	networks_cache = docker.Get_Networks(*this);
	return 0;
}

Labs_Core::Container Labs_Core::Container::Bogus(std::string id, std::string name, std::string image, std::string ip, std::vector<std::string> networks)
{
	Container bogus_container = Container(id);
	bogus_container.name_cache = name;
	bogus_container.image_cache = image;
	bogus_container.ip_cache = ip;
	bogus_container.networks_cache = networks;
	return bogus_container;
}
