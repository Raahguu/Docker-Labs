// Docker Labs.cpp : Defines the entry point for the application.
#include "main.h"
#include "cloudflare_hook.h"
#include "labs_user.h"
#include <iostream>
#include "labs_container.h"
#include "docker_cli.h"
#include "cloudflare_cli.h"
#include "global_cli.h"


int main(int argc, char* argv[])
{
	Docker_Labs::Labs_CLI::Command_Interpreter command = Docker_Labs::Labs_CLI::Command_Interpreter(argc, argv);

	//Section for cloudflare commands
	//./labs-cli cloudflare <command> [<subcommand>]

	if (command.Get_Partition() == "cloudflare") {
		return Docker_Labs::Labs_CLI::Cloudflare::Command_Handler(command, argc, argv);
	}
	else if (command.Get_Partition() == "docker") {
		return Docker_Labs::Labs_CLI::Docker::Command_Handler(command, argc, argv);
	}
	else {
		return Docker_Labs::Labs_CLI::Global_Handler(command, argc, argv);
	}

}


namespace Docker_Labs::Labs_CLI {


	Command_Interpreter::Command_Interpreter(int argc, char* argv[]) {

		if (argc < 2) {
			std::cerr << "Usage: labs-cli <partition> [command] [subcommand]\n";
		}
		else {

			partition = argv[1];

			int index = 2;

			// Detect first subcommand if it exists and does not start with '-'
			if (argc > index && argv[index][0] != '-') {
				command = argv[index];
				++index;
			}

			// Detect second subcommand if it exists and does not start with '-'
			if (argc > index && argv[index][0] != '-') {
				subcommand = argv[index];
				++index;
			}
		}
	}


	std::string Command_Interpreter::Get_Partition() {
		return partition;
	}

	std::string Command_Interpreter::Get_Command() {
		return command;
	}

	std::string Command_Interpreter::Get_SubCommand() {
		return subcommand;
	}

	std::vector<std::string> Command_Interpreter::Get_Flags() {
		return flags;
	}
}
