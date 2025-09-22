// Docker Labs.h : Include file for standard system include files,
// or project specific include files.
#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace Docker_Labs::Labs_CLI {
	class Command_Interpreter {
	public:
		Command_Interpreter(int argc, char* argv[]);
		std::string Get_Partition();
		std::string Get_Command();
		std::string Get_SubCommand();
		std::vector<std::string> Get_Flags();

	private:
		std::string partition;
		std::string command;
		std::string subcommand;
		std::vector<std::string> flags;
	};

}

// TODO: Reference additional headers your program requires here.
