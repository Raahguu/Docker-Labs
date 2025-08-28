#pragma once
#include <string>
#include <vector>
#include "labs_user.h"
#include "curl_wrapper.h"
#include <nlohmann/json.hpp>		

using json = nlohmann::json;

namespace Docker_Labs {

	class Container {
		public:
			explicit Container(std::string id);
			//Container(std::string container_name, std::string image_name);

			//Accessors
			std::string Get_ID();
			//std::string Get_Name();
			//std::string Get_Image();
			//std::string Get_IP();
			//std::vector<std::string> Get_Networks();
			//bool Get_Status();
			//
			//Cache Accessors
			std::string Get_Name_Cache();
			std::string Get_Image_Cache();
			std::string Get_IP_Cache();
			std::vector<std::string> Get_Networks_Cache();
			//
			int Cache_Update();
			static Container Bogus(std::string id, std::string name, std::string image, std::string ip, std::vector<std::string> networks);
			//
			////Controls
			//int Start();
			//int Stop();
			//int Restart();
			//int Kill();
			//int Remove();
			//json CallDockerAPI(const std::string& url, const std::string& data, std::string method);
			//int Test_API();
			//Docker_Labs::Curl_Wrapper curl;
	private:
			
			
			std::string id;
			std::string name_cache;
			std::string image_cache;
			std::string ip_cache;
			std::vector<std::string> networks_cache;
			
	};

	//namespace Docker::Commands {
	//	Container Get_Container(std::string container_name);
	//}

}
