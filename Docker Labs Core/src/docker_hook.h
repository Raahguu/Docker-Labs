#pragma once
#include "labs_container.h"

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

		json CallDockerAPI(const std::string& path, const std::string& data = "", std::string method = "GET");
		int Test_API();
		std::vector<Docker_Labs::Container> Get_All_Containers();
	private:
		Docker_Labs::Curl_Wrapper curl;
	};
}
