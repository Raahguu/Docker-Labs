#pragma once
#include <string>
#include <vector>
namespace Docker_Labs {
	class Container {
	public:
		Container(std::string docker_id, std::string name, std::string image, std::string ip_addr, std::string networks);
		std::string Get_Docker_ID();
		std::string Get_Name();
		void Set_Name(std::string name);
		std::string Get_Image();
		std::string Get_IP_Addr();
		std::vector<std::string> Get_Networks();
	private:
		const std::string docker_id;
		std::string name;
		const std::string image;
		std::string ip_addr;
		std::vector<std::string> networks;
	};
}
