// Docker Labs.cpp : Defines the entry point for the application.
//

#include "main.h"
#include "docker_hook.h"
#include "cloudflare_hook.h"

using namespace std;
using namespace Docker_Labs;

int main()
{
	User user;
	user.email = "rl";
	std::string image = "alpine";
	Docker_Labs::Docker::Create(user, image);
	return 0;
}

