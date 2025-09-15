#pragma once
#include <string>
#include <vector>
#include "labs_network.h"

namespace Docker_Labs::Labs_Core {

	class Container {
	public:
		explicit Container(std::string id);

		//Accessors
		std::string Get_ID();
		//Cache Accessors
		std::string Get_Name_Cache();
		std::string Get_Image_Cache();
		std::string Get_IP_Cache();
		std::vector<Labs_Core::Network> Get_Networks_Cache();
		int Cache_Update();
		static Container Bogus(std::string id, std::string name, std::string image, std::string ip, std::vector<Labs_Core::Network> networks);
		static Container Bogus(std::string id, std::string name, std::string image, std::string ip);

	private:
		std::string id;
		std::string name_cache;
		std::string image_cache;
		std::string ip_cache;
		std::vector<Labs_Core::Network> networks_cache;

	};

}
