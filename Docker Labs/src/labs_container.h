#pragma once
#include <string>

namespace Docker_Labs {
	class Container {
	public:
		Container(std::string docker_id, std::string name, std::string image, std::string ip_addr, std::string net_id);
		std::string Get_Docker_ID();
		std::string Get_Name();
		std::string Get_Image();
		std::string Get_IP_Addr();
		std::string Get_Net_ID();
	private:
		const std::string docker_id;
		const std::string name;
		const std::string image;
		const std::string ip_addr;
		const std::string net_id;
	};
}