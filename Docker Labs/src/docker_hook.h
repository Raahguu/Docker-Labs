#include "main.h"

namespace Docker_Labs::Docker {
	// Container management functions
	Container Create(std::string email, std::string image_name, std::string container_name);
	Container Create(User user, std::string image_name, std::string container_name);
	Container Create(std::string email, std::string image_name);
	Container Create(User user, std::string image_name);

	int Start(Container container);
	int Start(User user);

	int Stop(Container container);
	int Stop(User user);

	int Restart(Container container);
	int Restart(User user);

	int Kill(Container container);
	int Kill(User user);

	int Remove(Container container);
	int Remove(User user);

	// Getters
	std::string Get_IP(std::string container_name);
	std::string Get_IP(Container container);

	std::string Get_Image(std::string container_name);
	std::string Get_Image(Container container);

	std::string Get_Owner(std::string container_name);
	std::string Get_Owner(Container container);

	std::vector<std::string> Get_Networks(std::string container_name);
	std::vector<std::string> Get_Network(Container container);

	Container Get_Container(std::string container_name);

	bool Get_Status(std::string container_name);
	bool Get_Status(Container container);

	// Setters
	int Set_Owner(std::string container_name, std::string email);
	int Set_Owner(Container container, User user);

	// Utility
	std::vector<Container> Get_Owned_Containers(std::string email);
	std::vector<Container> Get_Owned_Containers(User user);
}
