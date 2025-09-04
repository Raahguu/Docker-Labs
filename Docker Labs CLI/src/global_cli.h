#pragma once
#include "main.h"
#include "cloudflare_hook.h"
#include "docker_hook.h"

namespace Docker_Labs::Labs_CLI {
	int Global_Handler(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Init_Handler(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, std::string email, std::string image);
	int Init_Handler(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, std::string email, std::string image, std::string container_name);
	int Rm_Handler(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, std::string container_name, bool keep_container);
	int Nuke(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, bool keep_containers);
}