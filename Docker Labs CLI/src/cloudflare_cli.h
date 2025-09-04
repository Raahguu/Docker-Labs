#pragma once
#include <string>
#include <tuple>
#include "main.h"
#include "cloudflare_hook.h"
#include "docker_hook.h"

namespace Docker_Labs::Labs_CLI::Cloudflare {
	int Command_Handler(Command_Interpreter command, int argc, char* argv[]);
	int Get_Seats(Docker_Labs::Cloudflare::API_Auth cf_auth);
	int Fetch_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth);
	int Fetch_DNS_Records(Docker_Labs::Cloudflare::API_Auth cf_auth);


	Container Spec_Container(int argc, char* argv[]);
	std::tuple<Container,User> Spec_Container_User(int argc, char* argv[]);

	int Create_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Update_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	int Create_DNS(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_DNS(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	int Create_Application(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Application(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	int Grant_Container(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Revoke_Container(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
}