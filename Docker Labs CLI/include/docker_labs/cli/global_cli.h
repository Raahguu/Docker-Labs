#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"

namespace Docker_Labs::Labs_CLI {
	int Global_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Init_Handler(int argc, char* argv[]);
	int Rm_Handler(int argc, char* argv[]);
	int Nuke(int argc, char* argv[]);
}