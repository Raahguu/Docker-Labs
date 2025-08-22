#pragma once
#include <string>
#include <vector>
#include "labs_user.h"
#include <nlohmann/json.hpp>		

using js0n = nlohmann::json;

namespace Docker_Labs {

	class Container {
		public:
			explicit Container(std::string id);
			Container(std::string email, std::string image_name, std::string container_name);
			Container(std::string email, std::string image_name);

			//Accessors
			std::string Get_ID();
			std::string Get_Name();
			std::string Get_Image();
			std::string Get_IP();
			std::vector<std::string> Get_Networks();
			std::string Get_Owner();
			bool Get_Status();

			//Cache Accessors
			std::string Get_Name_Cache();
			std::string Get_Image_Cache();
			std::string Get_IP_Cache();
			std::vector<std::string> Get_Networks_Cache();
			std::string Get_Owner_Cache();

			int Cache_Update();
			static Container Bogus(std::string id, std::string name, std::string image, std::string ip, std::vector<std::string> networks, std::string owner);

			//Controls
			int Start();
			int Stop();
			int Restart();
			int Kill();
			int Remove();
			int Set_Owner(std::string email);

	private:
			std::string id;
			std::string name_cache;
			std::string image_cache;
			std::string ip_cache;
			std::vector<std::string> networks_cache;
			std::string owner_cache;
	};

	namespace Docker {
		Container Get_Container(std::string container_name);
		std::vector<Container> Get_Owned_Containers(std::string email);
		int Start(std::string email);
		int Stop(std::string email);
		int Restart(std::string email);
		int Kill(std::string email);
		int Remove(std::string email);
		js0n CallDockerAPI(const std::string& url, const std::string& data, const std::string& method);
		uint64_t str_to_long(const std::string& str);
		std::string long_to_str(uint64_t value);

		int Test_API();
	}

}
