#include "main.h"
namespace Docker_Labs::Docker {
	Container Create(User user, std::string image_name);
	Container Create(User user, std::string image_name, std::string container_name);
	int Start(Container container);
	int Start(User user);
	int Stop(Container container);
	int Stop(User user);
	int Get_IP(Container container);
	int Set_Owner(Container container, User user);
	int Remove(Container container);
	int Get_Image(Container container);
	int Get_Status(Container container);
	int Get_Owner(Container container);
	int Reset(Container container);
}
