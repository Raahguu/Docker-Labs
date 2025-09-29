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

		/// <summary>
		/// Initializes a new instance of the <see cref=
		/// 
		/// /> class with the specified network ID.
		/// </summary>
		/// <param name="id">The Docker network ID.</param>
		explicit Network(std::string id);

		// ----------------------
		// Core Accessor
		// ----------------------

		/// <summary>
		/// Gets the network ID.
		/// </summary>
		/// <returns>The network ID string.</returns>
		std::string Get_ID();

		// ----------------------
		// Cached Metadata Accessors
		// ----------------------

		/// <summary>
		/// Gets the cached network name.
		/// </summary>
		/// <returns>The network name string.</returns>
		std::string Get_Name_Cache();

		/// <summary>
		/// Gets the cached subnet.
		/// </summary>
		/// <returns>The subnet string.</returns>
		std::string Get_Subnet_Cache();

		/// <summary>
		/// Gets the cached gateway.
		/// </summary>
		/// <returns>The gateway string.</returns>
		std::string Get_Gateway_Cache();

		/// <summary>
		/// Gets the cached IP range.
		/// </summary>
		/// <returns>The IP range string.</returns>
		std::string Get_IP_Range_Cache();

		/// <summary>
		/// Gets the cached list of containers associated with the network.
		/// </summary>
		/// <returns>Vector of container objects.</returns>
		std::vector<Labs_Core::Container> Get_Containers_Cache();

		/// <summary>
		/// Refreshes the cached metadata for the network from Docker.
		/// </summary>
		/// <returns>Status code.</returns>
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

}