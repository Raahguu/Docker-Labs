#pragma once
#include "labs_container.h"
#include "curl_wrapper.h"
#include "labs_network.h"
#include "json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Labs_Core {
	class Docker {
	public:
		Docker();

		Labs_Core::Container Get_Container(std::string container_name);
		std::string Get_ID(Labs_Core::Container container);
		std::string Get_Name(Labs_Core::Container container);
		std::string Get_Image(Labs_Core::Container container);
		std::string Get_IP(Labs_Core::Container container);
		std::vector<std::string> Get_Networks(Labs_Core::Container container);
		bool Get_Status(Labs_Core::Container container);
		Labs_Core::Container Create_Container(std::string container_name, std::string image_name);
		int Start(Labs_Core::Container container);
		int Stop(Labs_Core::Container container);
		int Restart(Labs_Core::Container container);
		int Kill(Labs_Core::Container container);
		int Remove(Labs_Core::Container container);
		Labs_Core::Container Reset(Labs_Core::Container container);

		//Network Stuff
		std::vector<std::string> Get_All_Networks();
		int Create_Network(std::string network_name, std::string subnet, std::string gateway, std::string IP_Range);
		int Delete_Network(Labs_Core::Network network);
		int Add_To_Network(Labs_Core::Network network);
		int Remove_From_Network(Labs_Core::Network network);
		std::string Get_ID(Labs_Core::Network network);
		std::string Get_Subnet(Labs_Core::Network network);
		std::string Get_Gateway(Labs_Core::Network network);
		std::string Get_IP_Range(Labs_Core::Network network);
		std::vector<Labs_Core::Container> Get_Networks_Container(Labs_Core::Network network);

		json CallDockerAPI(const std::string& path, const std::string& data = "", std::string method = "GET");
		int Test_API();
		std::vector<Labs_Core::Container> Get_All_Containers();
	private:
		Labs_Core::Curl_Wrapper curl;
	};
}
