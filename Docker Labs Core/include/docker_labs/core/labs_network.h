// -----------------------------------------------------------------------------
// @file labs_network.h
// @brief Declares the Network class for representing and managing Docker networks.
//
// The Network class represents a Docker network by its ID and provides access
// to cached properties such as name, subnet, gateway, and associated containers.
// This class is used to interface with Docker-managed networks and cache their state.
//
// Dependencies:
// - Forward declaration of Container (to avoid circular dependency)
//
// Namespace:
// - Docker_Labs::Labs_Core
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <vector>

namespace Docker_Labs::Labs_Core {

	class Container; // Forward declaration to resolve circular dependency

	class Network {
	public:
		// ----------------------
		// Constructor
		// ----------------------
		explicit Network(std::string id);

		// ----------------------
		// Core Accessor
		// ----------------------
		std::string Get_ID();

		// ----------------------
		// Cached Metadata Accessors
		// ----------------------
		std::string Get_Name_Cache();
		std::string Get_Subnet_Cache();
		std::string Get_Gateway_Cache();
		std::string Get_IP_Range_Cache();
		std::vector<Labs_Core::Container> Get_Containers_Cache();

		// Refresh cached metadata from Docker
		int Cache_Update();

	private:
		// ----------------------
		// Internal State
		// ----------------------
		std::string id;
		std::string name_cache;
		std::string subnet_cache;
		std::string gateway_cache;
		std::string IPRange;
		std::vector<Labs_Core::Container> containers_cache;
	};

} // namespace Docker_Labs::Labs_Core
