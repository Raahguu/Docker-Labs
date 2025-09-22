#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/labs_container.h"

namespace Docker_Labs::Labs_CLI::Cloudflare {
	Labs_Core::Cloudflare::API_Auth Get_Auth();
	Labs_Core::Cloudflare::API_Auth Get_Auth_Legacy();
	int Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Fetch_Seats(Labs_Core::Cloudflare::API_Auth cf_auth);
	int Fetch_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth);
	int Fetch_DNS_Records(Labs_Core::Cloudflare::API_Auth cf_auth);

	Labs_Core::Container Spec_Container(int argc, char* argv[]);
	std::vector<Labs_Core::User> Spec_User(int argc, char* argv[]);
	std::vector<Labs_Core::User_Seat> Spec_Seat(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	std::tuple<Labs_Core::Container, Labs_Core::User> Spec_Container_User(int argc, char* argv[]);

	int Create_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Update_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	int Create_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	int Create_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	int Grant_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Revoke_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Deactivate_Seats(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	int Help_Message(int argc, char* argv[]);
}