#include "labs_container.h"

Docker_Labs::Container::Container(std::string docker_id, std::string name, std::string image, std::string ip_addr, std::string net_id)
	: docker_id(docker_id), name(name), image(image), ip_addr(ip_addr), net_id(net_id)

{
}

std::string Docker_Labs::Container::Get_Docker_ID()
{
	return docker_id;
}

std::string Docker_Labs::Container::Get_Name()
{
	return name;
}

std::string Docker_Labs::Container::Get_Image()
{
	return image;
}

std::string Docker_Labs::Container::Get_IP_Addr()
{
	return ip_addr;
}

std::string Docker_Labs::Container::Get_Net_ID()
{
	return net_id;
}
