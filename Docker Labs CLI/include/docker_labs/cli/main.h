/**
 * main.h
 * Main entry point declarations for the Docker Labs CLI.
 *
 * Declares the main command interpreter class and core initialization
 * for the CLI application.
 */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace Docker_Labs::Labs_CLI {

	class Command_Interpreter {
	public:
		// Constructor that parses the CLI arguments.
		Command_Interpreter(int argc, char* argv[]);

		// Returns the top-level partition (e.g., "docker", "cloudflare").
		std::string Get_Partition();

		// Returns the main command (e.g., "start", "init", "remove").
		std::string Get_Command();

		// Returns the subcommand, if any (e.g., "all", "policy", etc.).
		std::string Get_SubCommand();

	private:
		std::string partition; // e.g., "docker", "cloudflare"
		std::string command; // e.g., "start", "add"
		std::string subcommand; // optional secondary command
	};

}


// TODO: Reference additional headers your program requires here.
