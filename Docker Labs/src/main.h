// Docker Labs.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <string>
#include <vector>

namespace Docker_Labs {
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
	
	struct Container {
		const std::string id;
        const std::string name;
        const std::string image;
        const std::string ip;
        const std::vector<std::string> networks;

		//Constructor
		Container(std::string id, std::string name, std::string image, std::string ip, std::vector<std::string> networks) 
			: id(id), name(name) image(image), ip(ip), networks(networks) {};	
	};
}
