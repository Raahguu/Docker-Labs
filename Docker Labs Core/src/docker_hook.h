#pragma once
#include "labs_container.h"
#include "curl_wrapper.h"
#include "labs_network.h"
#include "json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Docker {
	class Docker;

	class Docker {
	public:
		Docker();

		Docker_Labs::Container Get_Container(std::string container_name);
		std::string Get_ID(Docker_Labs::Container container);
		std::string Get_Name(Docker_Labs::Container container);
		std::string Get_Image(Docker_Labs::Container container);
		std::string Get_IP(Docker_Labs::Container container);
		std::vector<std::string> Get_Networks(Docker_Labs::Container container);
		bool Get_Status(Docker_Labs::Container container);
		Docker_Labs::Container Create_Container(std::string container_name, std::string image_name);
		int Start(Docker_Labs::Container container);
		int Stop(Docker_Labs::Container container);
		int Restart(Docker_Labs::Container container);
		int Kill(Docker_Labs::Container container);
		int Remove(Docker_Labs::Container container);
		Docker_Labs::Container Reset(Docker_Labs::Container container);

		//Network Stuff
		std::vector<std::string> Get_All_Networks();
		int Create_Network(std::string network_name, std::string subnet, std::string gateway, std::string IP_Range);
		int Delete_Network(Docker_Labs::Network network);
		int Add_To_Network(Docker_Labs::Network network);
		int Remove_From_Network(Docker_Labs::Network network);
		std::string Get_ID(Docker_Labs::Network network);
		std::string Get_Subnet(Docker_Labs::Network network);
		std::string Get_Gateway(Docker_Labs::Network network);
		std::string Get_IP_Range(Docker_Labs::Network network);
		std::vector<Docker_Labs::Container> Get_Networks_Container(Docker_Labs::Network network);

		json CallDockerAPI(const std::string& path, const std::string& data = "", std::string method = "GET");
		int Test_API();
		std::vector<Docker_Labs::Container> Get_All_Containers();
	private:
		Docker_Labs::Curl_Wrapper curl;
	};
}
