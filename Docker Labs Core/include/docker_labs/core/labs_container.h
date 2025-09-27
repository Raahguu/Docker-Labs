// -----------------------------------------------------------------------------
// @file labs_container.h
// @brief Declares the Container class for representing and managing Docker containers.
//
// The Container class encapsulates metadata and cached state about a Docker
// container, such as its ID, image, name, IP address, and network connections.
// It also provides simple lifecycle controls (Start, Stop, Remove).
//
// This is a lightweight representation that is backed by a container ID
// and optionally filled with cached values fetched from the Docker Engine.
//
// Dependencies:
// - labs_network.h (for container-network associations)
//
// Namespace:
// - Docker_Labs::Labs_Core
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <vector>
#include "docker_labs/core/labs_network.h"

namespace Docker_Labs::Labs_Core {

	class Container {
	public:
		// ----------------------
		// Constructor
		// ----------------------
		explicit Container(std::string id);

		// ----------------------
		// Core Accessors
		// ----------------------
		std::string Get_ID();

		// ----------------------
		// Cached Metadata
		// ----------------------
		std::string Get_Name_Cache();
		std::string Get_Image_Cache();
		std::string Get_IP_Cache();
		std::vector<Labs_Core::Network> Get_Networks_Cache();

		int Cache_Update();  // Refresh cached metadata

		// ----------------------
		// Lifecycle Controls
		// ----------------------
		int Start();
		int Stop();
		int Remove();

		// ----------------------
		// Static Utility
		// ----------------------
		static Container Bogus(std::string id, std::string name, std::string image, std::string ip, std::vector<Labs_Core::Network> networks);
		static Container Bogus(std::string id, std::string name, std::string image, std::string ip);

	private:
		// ----------------------
		// Internal State
		// ----------------------
		std::string id;
		std::string name_cache;
		std::string image_cache;
		std::string ip_cache;
		std::vector<Labs_Core::Network> networks_cache;
	};

} // namespace Docker_Labs::Labs_Core
