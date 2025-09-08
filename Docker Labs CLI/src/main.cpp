// Docker Labs.cpp : Defines the entry point for the application.
#include "main.h"
#include "global_cli.h"
#include "cloudflare_cli.h"
#include "docker_cli.h"

using namespace Docker_Labs;

int main(int argc, char* argv[])
{
	Labs_CLI::Command_Interpreter command = Labs_CLI::Command_Interpreter(argc, argv);

	//Section for cloudflare commands
	//./labs-cli cloudflare <command> [<subcommand>]

	if (command.Get_Partition() == "cloudflare") {
		return Labs_CLI::Cloudflare::Command_Handler(command, argc, argv);
	}
	else if (command.Get_Partition() == "docker") {
		return Labs_CLI::Docker::Command_Handler(command, argc, argv);
	}
	else {
		return Labs_CLI::Global_Handler(command, argc, argv);
	}

}

Labs_CLI::Command_Interpreter::Command_Interpreter(int argc, char* argv[]) {

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


std::string Labs_CLI::Command_Interpreter::Get_Partition() {
	return partition;
}

std::string Labs_CLI::Command_Interpreter::Get_Command() {
	return command;
}

std::string Labs_CLI::Command_Interpreter::Get_SubCommand() {
	return subcommand;
}

std::vector<std::string> Labs_CLI::Command_Interpreter::Get_Flags() {
	return flags;
}
