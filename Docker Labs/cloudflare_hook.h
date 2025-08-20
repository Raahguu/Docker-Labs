#include "main.h"

namespace Docker_Labs::Cloudflared {
	// Seats
	int Revoke_Seat(User user);
	// Ingress
	int Create_Ingress(Container container);
	int Delete_Ingress(Container container);
	// Appliaction
	int Create_Application(Container container);
	int Delete_Application(Container container);
	// Policy
	int Grant_Container(Container container, User user);
	int Revoke_Container(Container container, User user);
	std::vector<User> Get_Members(Container container);
	std::vector<Container> Get_Authorised_Containers(User user);
	// Global
	int Create(User user);
	int Delete(Container container);
	int Delete(User user);
}
