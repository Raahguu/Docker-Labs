#include "main.h"
#include "global_cli.h"


using namespace Docker_Labs;

int main(int argc, char* argv[])
{
	Labs_Web::Command_Interpreter command = Labs_Web::Command_Interpreter(argc, argv);

	//Section for cloudflare commands
	//./labs-cli cloudflare <command> [<subcommand>]

	
	return Labs_Web::Global_Handler(command, argc, argv);
	
}

Docker_Labs::Labs_Web::Command_Interpreter::Command_Interpreter(int argc, char* argv[]) {

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <partition> [command] [subcommand]\n";
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


std::string Docker_Labs::Labs_Web::Command_Interpreter::Get_Partition() {
	return partition;
}

std::string Docker_Labs::Labs_Web::Command_Interpreter::Get_Command() {
	return command;
}

std::string Docker_Labs::Labs_Web::Command_Interpreter::Get_SubCommand() {
	return subcommand;
}

std::vector<std::string> Docker_Labs::Labs_Web::Command_Interpreter::Get_Flags() {
	return flags;
}
