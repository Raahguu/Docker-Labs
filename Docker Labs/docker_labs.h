// Docker Labs.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <string>
#include <vector>

namespace Docker_Labs {
	class User {
		User();
		~User();
		/*std::string email;
		std::string cf_uid;*/
	};
	class Container {
		Container();
		~Container();
		/*std::string docker_id;
		std::string name;
		std::string image;
		std::string ip_addr;
		std::string net_id;*/
	};
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
