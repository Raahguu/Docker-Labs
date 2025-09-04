#include "labs_container.h"
#include "docker_hook.h"

namespace Docker_Labs {
	//Constructors
	Container::Container(std::string id) : id(id) {
	}

	////Gets
	std::string Container::Get_ID() {
		return this->id;
	}

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
}

