#include "docker_cli.h"
#include <string>
#include <map>
#include <getopt.h>
#include <iostream>
#include <vector>
#include "labs_container.h"

using json = nlohmann::json;
using namespace std::string_view_literals;

int Docker_Labs::Docker::Commands::Test_API(Docker_Labs::Command_Interpreter command, int argc, char* argv[]){
	int returnCode = 0;	

	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'}, // --help maps to 'h'
		{"code", no_argument, nullptr, 'c'}, // --code maps to 'c'
		{nullptr, 0, nullptr, 0}
	};
	
	int opt;
	while((opt=getopt_long(argc, argv, "hc", long_flags, nullptr)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "test-api:\n\tTests that the HTTP API requests for docker are working" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "-h, --help\t\tProvides help on what flags this command offers" << std::endl;
				std::cout << "-c, --code\t\tOutputs the exact HTTP code the API returns with rather then a human readable message" << std::endl;
				return 0;
			case 'c':
				returnCode = 1;
				continue;
			case 0:
				continue;
			case '?':
			default:
				std::string temp = argv[optind - 1];
				while (!temp.empty() && temp[0] == '-') {
					temp.erase(0, 1);
				}
				std::cerr << "Unknown flag: " << temp << std::endl;
				return 1;
		}
	}
	
	int result = Docker_Labs::Docker::Test_API();
	
	if (returnCode){
		std::cout << result << std::endl;
		return 0;
	}

	if(result == 200) {
		std::cout << "Docker HTTP API responded successfully" << std::endl;
		return 0;
	}
	
	if(result == 400) {
		std::cerr << "Docker version does not support HTTP API v1.51, please update Docker" << std::endl;
		return 1;
	}
	
	std::cerr << "API did not respond" << std::endl; // The API should only respond with a 200 or 400
	return 1;
}


int Docker_Labs::Docker::Commands::Test_Container_Creation(Docker_Labs::Command_Interpreter command, int argc, char* argv[]){
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'}, // --help maps to 'h'
		{"name", required_argument, nullptr, 0}, // --name
		{"image", required_argument, nullptr, 0},
		{nullptr, 0, nullptr, 0}
	};
	
	std::string name = "";
	std::string image = "";
	
	int opt;
	int long_index;
	while((opt=getopt_long(argc, argv, "h", long_flags, &long_index)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "test-container-creation:\n\tTests if the container creation is working correctly with docker. Should create a container" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "-h, --help\t\tProvides help on what flags this command offers" << std::endl;
				std::cout << "--name\t\t[Required] Lets you change the exact name of the container" << std::endl;
				std::cout << "--image\t\t[Required] Lets you change the exact image of the container" << std::endl;
				return 0;
			case 0: // no short flag option
				if (std::string(long_flags[long_index].name) == "name") {
					name = optarg;
				}
				else if (std::string(long_flags[long_index].name) == "image") {
					image = optarg;
				}
				continue;
			case '?':
			default:
				std::string temp = argv[optind - 1];
				while (!temp.empty() && temp[0] == '-') {
					temp.erase(0, 1);
				}
				std::cerr << "Unknown flag: " << temp << std::endl;
				return 1;
		}
	}
	
	if(image == ""){
		std::cerr << "You must provide an image" << std::endl;
		return 1;
	}
	if(name == ""){
		std::cerr << "You must provide a name" << std::endl;
	}
	
	
	Container result = Container(name, image);
	result.Start();
	
	std::string id = result.Get_ID();
	std::cout << "Container Id: " << id << std::endl;
	std::string container_name = result.Get_Name();
	std::cout << "Container Name: " << container_name << std::endl;
	std::string ip = result.Get_IP();
	std::cout << "Container IP: " << ip << std::endl;
	image = result.Get_Image();
	std::cout << "Container Image: " << image << std::endl;
	std::string status = result.Get_Status() ? "up" : "down";
	std::cout << "Container Status: " << status << std::endl;
	std::vector<std::string> networks = result.Get_Networks();
	std::cout << "Container Networks: " << std::endl;
	for(std::string& network : networks){
		std::cout << network << std::endl;
	}
	
	result.Restart();
	result.Stop();
	
	std::cout << "Checking getting the container with the name" << std::endl;
	if(Docker_Labs::Docker::Get_Container(name).Get_ID() == result.Get_ID()){
		std::cout << "Success!" << std::endl;
	} else {
		std::cout << "Failed" << std::endl;
	}
	
	result.Start();
	result.Kill();
	result.Remove();
	
	return 0;
}

int Docker_Labs::Docker::Commands::Test_Handler(Docker_Labs::Command_Interpreter command, int argc, char* argv[]){
	opterr = 0; // remove getopt's custom error message when an incorrect flag is supplied

	static std::map<std::string_view, std::function<int(Docker_Labs::Command_Interpreter, int, char**)>> possible_commands = {
		{"api"sv, Docker_Labs::Docker::Commands::Test_API},
		{"container-creation"sv, Docker_Labs::Docker::Commands::Test_Container_Creation}
	};

	auto it = possible_commands.find(command.Get_SubCommand());
	
	if (it != possible_commands.end()){
		return it->second(command, argc, argv);
	} else {
		std::cerr << "Unknown command: " << command.Get_SubCommand() << std::endl;
		std::cerr << "Run '--help' or '-h' for help finding a command" << std::endl;
		return 1;
	}
	return 1;
}

int Docker_Labs::Docker::Commands::Help(Docker_Labs::Command_Interpreter command, int argc, char* argv[]){
	std::cout << "./labs-cli docker <command> [<subcommand>] [<flags>]" << std::endl;
	std::cout << std::endl;
	std::cout << "The current available commands are:" << std::endl;
	std::cout << "\thelp\t-\tDisplays this help message" << std::endl;
	std::cout << "\ttest - group of commands for testing thr program works correctly" << std::endl;
	std::cout << "\t\tapi\t-\tTests that the HTTP API requests for docker are working" << std::endl;
	std::cout << "\t\tcontainer-creation\t-\tTests if the container creation is working correctly with docker. Should create a container" << std::endl;
	std::cout << std::endl;
	std::cout << "Run './labs-cli docker <command> --help' for more info on a command" << std::endl;
	return 0;
}

int Docker_Labs::Docker::Commands::Command_Handler(Docker_Labs::Command_Interpreter command, int argc, char* argv[]){
	opterr = 0; // remove getopt's custom error message when an incorrect flag is supplied

	static std::map<std::string_view, std::function<int(Docker_Labs::Command_Interpreter, int, char**)>> possible_commands = {
		{"test-api"sv, Docker_Labs::Docker::Commands::Test_API},
		{"help"sv, Docker_Labs::Docker::Commands::Help},
		{"test"sv, Docker_Labs::Docker::Commands::Test_Handler}
	};

	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'}, // --help maps to 'h'
		{nullptr, 0, nullptr, 0}
	};

	auto it = possible_commands.find(command.Get_Command());
	
	if (it != possible_commands.end()){
		return it->second(command, argc, argv);
	} else if (command.Get_Command() == ""){
		int opt;
		while((opt=getopt_long(argc, argv, "h", long_flags, nullptr)) != -1) {
			switch (opt) {
				case 'h':
					return possible_commands["help"](command, argc, argv);
				case '?':
				default:
					std::string temp = argv[optind - 1];
					while (!temp.empty() && temp[0] == '-') {
						temp.erase(0, 1);
					}
					std::cout << "Unknown flag: " << temp << std::endl;
					return 1;
			}
		}
	} else {
		std::cerr << "Unknown command: " << command.Get_Command() << std::endl;
		std::cerr << "Run '--help' or '-h' for help finding a command" << std::endl;
		return 1;
	}
	return 1;
}

	/*
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'}, // --help maps to 'h'
		{"all", no_argument, nullptr, 'a'}, // --all maps to 'a'
		{"name", required_argument, nullptr, 0}, // --name is a custom long flag
		{nullptr, 0, nullptr, 0}
	};

	int opt;
	int long_index = 0;
			
	while((opt=getopt_long(argc, argv, "ab:h", long_flags, &long_index)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "You asked for help" << std::endl;
				break;
			case '?':
			default:
				std::cout << "Unknown flag: " << (char)optopt << std::endl;
				return 1;
				
			case 0: // no short flag option
				if (std::string(long_flags[long_index].name) == "name") {
					std::cout << "flag name with value " << optarg << std::endl;
				}
				continue;
			case 'a':
				std::cout << "flag a" << std::endl;
				continue;
			case 'b':
				std::cout << "flag b with value " << optarg << std::endl;
				continue;
		}
	}*/
