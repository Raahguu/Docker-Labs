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

		/// <summary>
		/// Initializes a new instance of the <see cref="Container"/> class with the specified container ID.
		/// </summary>
		/// <param name="id">The Docker container ID.</param>
		explicit Container(std::string id);

		// ----------------------
		// Core Accessors
		// ----------------------

		/// <summary>
		/// Gets the container ID.
		/// </summary>
		/// <returns>The container ID string.</returns>
		std::string Get_ID();

		// ----------------------
		// Cached Metadata
		// ----------------------

		/// <summary>
		/// Gets the cached container name.
		/// </summary>
		/// <returns>The container name string.</returns>
		std::string Get_Name_Cache();

		/// <summary>
		/// Gets the cached image name.
		/// </summary>
		/// <returns>The image name string.</returns>
		std::string Get_Image_Cache();

		/// <summary>
		/// Gets the cached IP address.
		/// </summary>
		/// <returns>The IP address string.</returns>
		std::string Get_IP_Cache();

		/// <summary>
		/// Gets the cached list of networks associated with the container.
		/// </summary>
		/// <returns>Vector of network objects.</returns>
		std::vector<Labs_Core::Network> Get_Networks_Cache();

		/// <summary>
		/// Refreshes the cached metadata for the container.
		/// </summary>
		/// <returns>Status code.</returns>
		int Cache_Update();

		// ----------------------
		// Lifecycle Controls
		// ----------------------

		/// <summary>
		/// Starts the container.
		/// </summary>
		/// <returns>Status code.</returns>
		int Start();

		/// <summary>
		/// Stops the container.
		/// </summary>
		/// <returns>Status code.</returns>
		int Stop();

		/// <summary>
		/// Removes the container.
		/// </summary>
		/// <returns>Status code.</returns>
		int Remove();

		// ----------------------
		// Static Utility
		// ----------------------

		/// <summary>
		/// Creates a bogus (mock) container with the specified metadata and networks.
		/// </summary>
		/// <param name="id">The container ID.</param>
		/// <param name="name">The container name.</param>
		/// <param name="image">The image name.</param>
		/// <param name="ip">The IP address.</param>
		/// <param name="networks">Vector of network objects.</param>
		/// <returns>A mock container object.</returns>
		static Container Bogus(std::string id, std::string name, std::string image, std::string ip, std::vector<Labs_Core::Network> networks);

		/// <summary>
		/// Creates a bogus (mock) container with the specified metadata and no networks.
		/// </summary>
		/// <param name="id">The container ID.</param>
		/// <param name="name">The container name.</param>
		/// <param name="image">The image name.</param>
		/// <param name="ip">The IP address.</param>
		/// <returns>A mock container object.</returns>
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

}