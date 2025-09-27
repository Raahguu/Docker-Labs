// -----------------------------------------------------------------------------
// @file docker.h
// @brief Declares the Docker class for managing Docker containers and networks
//
// This class abstracts the interaction with the Docker Engine API through
// local UNIX socket requests. It provides methods for creating, inspecting,
// modifying, and deleting Docker containers and networks.
//
// Dependencies:
// - labs_container.h (Container model)
// - labs_network.h (Network model)
// - curl_wrapper.h (for HTTP/Socket requests)
// - nlohmann::json (for parsing Docker API responses)
//
// Namespace:
// - Docker_Labs::Labs_Core
// -----------------------------------------------------------------------------

#pragma once
#include "docker_labs/core/labs_container.h"
#include "docker_labs/core/labs_network.h"
#include "docker_labs/core/curl_wrapper.h"
#include "json/json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Labs_Core {

	class Docker {
	public:
		// ----------------------
		// Constructor
		// ----------------------
		Docker();

		// ----------------------
		// Container Management
		// ----------------------
		Labs_Core::Container Get_Container(std::string container_name);
		Labs_Core::Container Create_Container(std::string container_name, std::string image_name);
		Labs_Core::Container Reset(Labs_Core::Container container);
		std::vector<Labs_Core::Container> Get_All_Containers();

		// ----------------------
		// Container Attributes
		// ----------------------
		std::string Get_Container_ID(Labs_Core::Container container);
		std::string Get_Container_Name(Labs_Core::Container container);
		std::string Get_Image(Labs_Core::Container container);
		std::string Get_IP(Labs_Core::Container container);
		std::vector<Labs_Core::Network> Get_Networks(Labs_Core::Container container);
		bool Get_Status(Labs_Core::Container container);

		// ----------------------
		// Container Lifecycle
		// ----------------------
		int Start(Labs_Core::Container container);
		int Stop(Labs_Core::Container container);
		int Restart(Labs_Core::Container container);
		int Kill(Labs_Core::Container container);
		int Remove(Labs_Core::Container container);
		int Remove(Labs_Core::Container container, bool remove_network);

		// ----------------------
		// Network Management
		// ----------------------
		std::vector<Labs_Core::Network> Get_All_Networks();
		Labs_Core::Network Create_Network(std::string network_name);
		Labs_Core::Network Create_Network(std::string network_name, std::string subnet, std::string gateway, std::string IP_Range);
		int Delete_Network(Labs_Core::Network network);
		int Add_To_Network(Labs_Core::Network network, Labs_Core::Container container);
		int Remove_From_Network(Labs_Core::Network network, Labs_Core::Container container);

		// ----------------------
		// Network Attributes
		// ----------------------
		std::string Get_Network_ID(Labs_Core::Network network);
		std::string Get_Network_Name(Labs_Core::Network network);
		std::string Get_Subnet(Labs_Core::Network network);
		std::string Get_Gateway(Labs_Core::Network network);
		std::string Get_IP_Range(Labs_Core::Network network);
		std::vector<Labs_Core::Container> Get_Containers(Labs_Core::Network network);
		Labs_Core::Network Get_Network(std::string network_name);

		// ----------------------
		// Raw Docker API Access
		// ----------------------
		json CallDockerAPI(const std::string& path);
		json CallDockerAPI(const std::string& path, const std::string& data, std::string method);
		int Test_API();

	private:
		Labs_Core::Curl_Wrapper curl;
	};

}	
