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

		/// <summary>
		/// Initializes a new instance of the <see cref="Docker"/> class.
		/// </summary>
		Docker();

		// ----------------------
		// Container Management
		// ----------------------

		/// <summary>
		/// Retrieves a container by name.
		/// </summary>
		/// <param name="container_name">The name of the container.</param>
		/// <returns>The container object.</returns>
		Labs_Core::Container Get_Container(std::string container_name);

		/// <summary>
		/// Creates a new container with the specified name and image.
		/// </summary>
		/// <param name="container_name">The name of the container.</param>
		/// <param name="image_name">The Docker image name.</param>
		/// <returns>The created container object.</returns>
		Labs_Core::Container Create_Container(std::string container_name, std::string image_name);

		/// <summary>
		/// Creates a new container with the specified name and image.
		/// </summary>
		/// <param name="container_name">The name of the container.</param>
		/// <param name="image_name">The Docker image name.</param>
		/// <param name="internal_net">Whether to use an internal network.</param>
		/// <returns>The created container object.</returns>
		Labs_Core::Container Create_Container(std::string container_name, std::string image_name, bool internal_net);

		/// <summary>
		/// Resets a container to its initial state.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>The reset container object.</returns>
		Labs_Core::Container Reset(Labs_Core::Container container);

		/// <summary>
		/// Retrieves all containers.
		/// </summary>
		/// <returns>Vector of all container objects.</returns>
		std::vector<Labs_Core::Container> Get_All_Containers();

		// ----------------------
		// Container Attributes
		// ----------------------

		/// <summary>
		/// Gets the ID of a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Container ID string.</returns>
		std::string Get_Container_ID(Labs_Core::Container container);

		/// <summary>
		/// Gets the name of a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Container name string.</returns>
		std::string Get_Container_Name(Labs_Core::Container container);

		/// <summary>
		/// Gets the image name used by a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Image name string.</returns>
		std::string Get_Image(Labs_Core::Container container);

		/// <summary>
		/// Gets the IP address of a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>IP address string.</returns>
		std::string Get_IP(Labs_Core::Container container);

		/// <summary>
		/// Gets the networks attached to a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Vector of network objects.</returns>
		std::vector<Labs_Core::Network> Get_Networks(Labs_Core::Container container);

		/// <summary>
		/// Gets the running status of a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>True if running, otherwise false.</returns>
		bool Get_Status(Labs_Core::Container container);

		// ----------------------
		// Container Lifecycle
		// ----------------------

		/// <summary>
		/// Starts a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Start(Labs_Core::Container container);

		/// <summary>
		/// Stops a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Stop(Labs_Core::Container container);

		/// <summary>
		/// Restarts a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Restart(Labs_Core::Container container);

		/// <summary>
		/// Kills a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Kill(Labs_Core::Container container);

		/// <summary>
		/// Removes a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Remove(Labs_Core::Container container);

		/// <summary>
		/// Removes a container, with an option to remove its network.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="remove_network">Whether to remove the associated network.</param>
		/// <returns>Status code.</returns>
		int Remove(Labs_Core::Container container, bool remove_network);

		// ----------------------
		// Network Management
		// ----------------------

		/// <summary>
		/// Retrieves all Docker networks.
		/// </summary>
		/// <returns>Vector of all network objects.</returns>
		std::vector<Labs_Core::Network> Get_All_Networks();

		/// <summary>
		/// Creates a new Docker network with the specified name.
		/// </summary>
		/// <param name="network_name">The network name.</param>
		/// <returns>The created network object.</returns>
		Labs_Core::Network Create_Network(std::string network_name);

		/// <summary>
		/// Creates a new Docker network with the specified name.
		/// </summary>
		/// <param name="network_name">The network name.</param>
		/// <param name="internal_net">Whether to use an internal network.</param>
		/// <returns>The created network object.</returns>
		Labs_Core::Network Create_Network(std::string network_name, bool internal_net);

		/// <summary>
		/// Creates a new Docker network with custom subnet, gateway, and IP range.
		/// </summary>
		/// <param name="network_name">The network name.</param>
		/// <param name="subnet">The subnet.</param>
		/// <param name="gateway">The gateway.</param>
		/// <param name="IP_Range">The IP range.</param>
		/// <returns>The created network object.</returns>
		Labs_Core::Network Create_Network(std::string network_name, std::string subnet, std::string gateway, std::string IP_Range);

		/// <summary>
		/// Creates a new Docker network with custom subnet, gateway, and IP range.
		/// </summary>
		/// <param name="network_name">The network name.</param>
		/// <param name="subnet">The subnet.</param>
		/// <param name="gateway">The gateway.</param>
		/// <param name="IP_Range">The IP range.</param>
		/// <param name="internal_net">Whether to use an internal network.</param>
		/// <returns>The created network object.</returns>
		Labs_Core::Network Create_Network(std::string network_name, std::string subnet, std::string gateway, std::string IP_Range, bool internal_net);

		/// <summary>
		/// Deletes a Docker network.
		/// </summary>
		/// <param name=
		/// >The network object.</param>
		/// <returns>Status code.</returns>
		int Delete_Network(Labs_Core::Network network);

		/// <summary>
		/// Adds a container to a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Add_To_Network(Labs_Core::Network network, Labs_Core::Container container);

		/// <summary>
		/// Removes a container from a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Remove_From_Network(Labs_Core::Network network, Labs_Core::Container container);

		// ----------------------
		// Network Attributes
		// ----------------------

		/// <summary>
		/// Gets the ID of a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <returns>Network ID string.</returns>
		std::string Get_Network_ID(Labs_Core::Network network);

		/// <summary>
		/// Gets the name of a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <returns>Network name string.</returns>
		std::string Get_Network_Name(Labs_Core::Network network);

		/// <summary>
		/// Gets the subnet of a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <returns>Subnet string.</returns>
		std::string Get_Subnet(Labs_Core::Network network);

		/// <summary>
		/// Gets the gateway of a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <returns>Gateway string.</returns>
		std::string Get_Gateway(Labs_Core::Network network);

		/// <summary>
		/// Gets the IP range of a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <returns>IP range string.</returns>
		std::string Get_IP_Range(Labs_Core::Network network);

		/// <summary>
		/// Gets all containers attached to a network.
		/// </summary>
		/// <param name="network">The network object.</param>
		/// <returns>Vector of container objects.</returns>
		std::vector<Labs_Core::Container> Get_Containers(Labs_Core::Network network);

		/// <summary>
		/// Retrieves a network by name.
		/// </summary>
		/// <param name="network_name">The network name.</param>
		/// <returns>The network object.</returns>
		Labs_Core::Network Get_Network(std::string network_name);

		// ----------------------
		// Raw Docker API Access
		// ----------------------

		/// <summary>
		/// Calls the Docker API with the specified path (GET).
		/// </summary>
		/// <param name="path">The API path.</param>
		/// <returns>JSON response from the API.</returns>
		json CallDockerAPI(const std::string& path);

		/// <summary>
		/// Calls the Docker API with the specified path, data, and method.
		/// </summary>
		/// <param name="path">The API path.</param>
		/// <param name="data">Payload to send in the request body.</param>
		/// <param name="method">HTTP method (e.g., "POST", "PUT").</param>
		/// <returns>JSON response from the API.</returns>
		json CallDockerAPI(const std::string& path, const std::string& data, std::string method);

		/// <summary>
		/// Tests Docker API connectivity.
		/// </summary>
		/// <returns>Status code.</returns>
		int Test_API();

	private:
		Labs_Core::Curl_Wrapper curl;
	};

}

