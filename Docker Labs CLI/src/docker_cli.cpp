#include "docker_cli.h"
#include "docker_hook.h"

int Docker_Labs::Labs_CLI::Docker::Test_API(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){
	int returnCode = 0;	

	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'}, // --help maps to 'h'
		{"code", no_argument, nullptr, 'c'}, // --code maps to 'c'
		{nullptr, 0, nullptr, 0}
	};
	
	int opt;
	while((opt=getopt_long(argc, argv, "hc", long_flags, nullptr)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "Tests that the HTTP API requests for docker are working" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "\t-h, --help: Provides help on what flags this command offers" << std::endl;
				std::cout << "\t-c, --code: Outputs the exact HTTP code the API returns with rather then a human readable message" << std::endl;
				return 0;
			case 'c':
				returnCode = 1;
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
	
	int result = docker.Test_API();
	
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


int Docker_Labs::Labs_CLI::Docker::Test_Container_Control(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){
	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'},
		{"name", required_argument, nullptr, 'n'}, 
		{"image", required_argument, nullptr, 'i'},
		{nullptr, 0, nullptr, 0}
	};
	
	std::string name = "";
	std::string image = "";
	
	int opt;
	while((opt=getopt_long(argc, argv, "hn:i:", long_flags, nullptr)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "Tests if the container control commands are working correctly" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "\t-h, --help: Provides help on what flags this command offers" << std::endl;
				std::cout << "\t-n, --name: [Required] Lets you change the exact name of the container" << std::endl;
				std::cout << "\t-i, --image: [Required] Lets you change the exact image of the container" << std::endl;
				return 0;
			case 'n': 
				name = optarg;
				continue;
			case 'i':
				image = optarg;
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
		return 1;
	}
	
	
	Container result = docker.Create_Container(name, image);
	docker.Start(result);
	
	std::string id = docker.Get_ID(result);
	std::cout << "Container Id: " << id << std::endl;
	std::string container_name = docker.Get_Name(result);
	std::cout << "Container Name: " << container_name << std::endl;
	std::string ip = docker.Get_IP(result);
	std::cout << "Container IP: " << ip << std::endl;
	image = docker.Get_Image(result);
	std::cout << "Container Image: " << image << std::endl;
	std::string status = docker.Get_Status(result) ? "up" : "down";
	std::cout << "Container Status: " << status << std::endl;
	std::vector<std::string> networks = docker.Get_Networks(result);
	std::cout << "Container Networks: " << std::endl;
	for(std::string& network : networks){
		std::cout << network << std::endl;
	}
	
	docker.Restart(result);
	docker.Stop(result);
	
	std::cout << "Checking getting the container with the name" << std::endl;
	if (docker.Get_Container(name).Get_ID() == docker.Get_ID(result)) {
		std::cout << "Success!" << std::endl;
	} else {
		std::cout << "Failed" << std::endl;
	}
	
	docker.Start(result);	
	result = docker.Reset(result);
	docker.Kill(result);
	docker.Remove(result);
	return 0;
}


int Docker_Labs::Labs_CLI::Docker::Test_Handler(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){
	opterr = 0; // remove getopt's custom error message when an incorrect flag is supplied


	static std::map<std::string_view, std::function<int(Docker_Labs::Labs_CLI::Command_Interpreter, int, char**)>> possible_commands = {
		{"api", Docker_Labs::Labs_CLI::Docker::Test_API},
		{"container-control", Docker_Labs::Labs_CLI::Docker::Test_Container_Control}
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

int Docker_Labs::Labs_CLI::Docker::Help(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){
	std::cout << "./labs-cli docker <command> [<subcommand>] [<flags>]" << std::endl;
	std::cout << std::endl;
	std::cout << "The current available commands are:" << std::endl;
	std::cout << "\thelp - Displays this help message" << std::endl;
	std::cout << "\ttest - group of commands for testing thr program works correctly" << std::endl;
	std::cout << "\t\tapi - Tests that the HTTP API requests for docker are working" << std::endl;
	std::cout << "\t\tcontainer-control - Tests if the container controls are working" << std::endl;
	std::cout << std::endl;
	std::cout << "Run './labs-cli docker <command> --help' for more info on a command" << std::endl;
	return 0;
}

int Docker_Labs::Labs_CLI::Docker::Start(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){	
	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'},
		{"name", required_argument, nullptr, 'n'}, 
		{nullptr, 0, nullptr, 0}
	};
	
	std::string name = "";
	
	int opt;
	while((opt=getopt_long(argc, argv, "hn:", long_flags, nullptr)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "Starts the specified container" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "\t-h, --help: Provides help on what flags this command offers" << std::endl;
				std::cout << "\t-n, --name: [Required] Lets you specify the name of the container" << std::endl;
				return 0;
			case 'n': 
				name = optarg;
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
	
	if(name == ""){
		std::cerr << "You must provide a name" << std::endl;
		return 1;
	}
	
	
	Container result = docker.Get_Container(name);
	docker.Start(result);
	return 0;
}


int Docker_Labs::Labs_CLI::Docker::Stop(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){	
	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'},
		{"name", required_argument, nullptr, 'n'}, 
		{nullptr, 0, nullptr, 0}
	};
	
	std::string name = "";
	
	int opt;
	while((opt=getopt_long(argc, argv, "hn:", long_flags, nullptr)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "Stops the specified container" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "\t-h, --help: Provides help on what flags this command offers" << std::endl;
				std::cout << "\t-n, --name: [Required] Lets you specify the name of the container" << std::endl;
				return 0;
			case 'n': 
				name = optarg;
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
	
	if(name == ""){
		std::cerr << "You must provide a name" << std::endl;
		return 1;
	}
	
	
	Container result = docker.Get_Container(name);
	docker.Stop(result);
	return 0;
}


int Docker_Labs::Labs_CLI::Docker::Reset(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){	
	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'},
		{"name", required_argument, nullptr, 'n'}, 
		{nullptr, 0, nullptr, 0}
	};
	
	std::string name = "";
	
	int opt;
	while((opt=getopt_long(argc, argv, "hn:", long_flags, nullptr)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "Resets the specified container back to its default state" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "\t-h, --help: Provides help on what flags this command offers" << std::endl;
				std::cout << "\t-n, --name: [Required] Lets you specify the name of the container" << std::endl;
				return 0;
			case 'n': 
				name = optarg;
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
	
	if(name == ""){
		std::cerr << "You must provide a name" << std::endl;
		return 1;
	}
	
	
	Container result = docker.Get_Container(name);
	docker.Reset(result);
	return 0;
}

int Docker_Labs::Labs_CLI::Docker::Restart(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){	
	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	static struct option long_flags[] = {
		{"help", no_argument, nullptr, 'h'},
		{"name", required_argument, nullptr, 'n'}, 
		{nullptr, 0, nullptr, 0}
	};
	
	std::string name = "";
	
	int opt;
	while((opt=getopt_long(argc, argv, "hn:", long_flags, nullptr)) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "Restarts the specified container" << std::endl;
				std::cout << "Available flags:" << std::endl;
				std::cout << "\t-h, --help: Provides help on what flags this command offers" << std::endl;
				std::cout << "\t-n, --name: [Required] Lets you specify the name of the container" << std::endl;
				return 0;
			case 'n': 
				name = optarg;
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
	
	if(name == ""){
		std::cerr << "You must provide a name" << std::endl;
		return 1;
	}
	
	
	Container result = docker.Get_Container(name);
	docker.Restart(result);
	return 0;
}

int Docker_Labs::Labs_CLI::Docker::Command_Handler(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]){
	opterr = 0; // remove getopt's custom error message when an incorrect flag is supplied


	static std::map<std::string_view, std::function<int(Docker_Labs::Labs_CLI::Command_Interpreter, int, char**)>> possible_commands = {
		{"help", Docker_Labs::Labs_CLI::Docker::Help},
		{"test", Docker_Labs::Labs_CLI::Docker::Test_Handler},
		{"start", Docker_Labs::Labs_CLI::Docker::Start},
		{"stop", Docker_Labs::Labs_CLI::Docker::Stop},
		{"restart", Docker_Labs::Labs_CLI::Docker::Restart},
		{"reset", Docker_Labs::Labs_CLI::Docker::Reset}
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
