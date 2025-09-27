/**
 * docker_cli.cpp
 * Implementation of Docker CLI command handlers.
 *
 * Implements the logic for managing Docker container commands
 * through the CLI interface.
 */

#include "docker_labs/cli/docker_cli.h"
#include "docker_labs/core/docker_hook.h"
#include <boost/program_options.hpp>

using namespace Docker_Labs;
namespace po = boost::program_options;

// Function to test Docker HTTP API communication
int Labs_CLI::Docker::Test_API(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	bool output_http_code = 0;

	Labs_Core::Docker docker = Labs_Core::Docker();

	// Define command line options for this command
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers")
		("code,c", "Outputs the exact HTTP code the API returns rather than a human-readable message");

	po::variables_map vm;
	try {
		// Parse command line options
		po::store(po::parse_command_line(argc, argv, desc), vm);
		if (vm.count("help")) {
			std::cout << "Tests that the HTTP API requests for docker are working" << std::endl;
			std::cout << desc << std::endl;
			return 0;
		}
		po::notify(vm); // Throws if required options are missing
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (vm.count("code")) {
		output_http_code = true; // Flag to output HTTP code instead of human message
	}

	int result = docker.Test_API(); // Run the API test

	if (output_http_code) {
		std::cout << result << std::endl;
		return 0;
	}

	// Output messages based on returned HTTP code
	if (result == 200) {
		std::cout << "Docker HTTP API responded successfully" << std::endl;
		return 0;
	}

	if (result == 400) {
		std::cerr << "Docker version does not support HTTP API v1.51, please update Docker" << std::endl;
		return 1;
	}

	std::cerr << "API did not respond" << std::endl; // Only expecting 200 or 400
	return 1;
}

// Function to test container creation and control commands
int Labs_CLI::Docker::Test_Container_Control(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	Labs_Core::Docker docker = Labs_Core::Docker();
	std::string name = "";
	std::string image = "";

	// Define options with required parameters for container name and image
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers")
		("name,n", po::value<std::string>(&name)->required(), "Lets you change the exact name of the container")
		("image,i", po::value<std::string>(&image)->required(), "Lets you change the exact image of the container");

	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm); // Throws if required options are missing
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (vm.count("help")) {
		std::cout << "Tests if the container control commands are working correctly" << std::endl;
		std::cout << "Available flags:" << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	// Create and start container with provided name and image
	Labs_Core::Container result = docker.Create_Container(name, image);
	docker.Start(result);

	// Output container details
	std::string id = docker.Get_Container_ID(result);
	std::cout << "Container Id: " << id << std::endl;
	std::string container_name = docker.Get_Container_Name(result);
	std::cout << "Container Name: " << container_name << std::endl;
	std::string ip = docker.Get_IP(result);
	std::cout << "Container IP: " << ip << std::endl;
	image = docker.Get_Image(result);
	std::cout << "Container Image: " << image << std::endl;
	std::string status = docker.Get_Status(result) ? "up" : "down";
	std::cout << "Container Status: " << status << std::endl;

	// List the container's networks
	std::vector<Labs_Core::Network> networks = docker.Get_Networks(result);
	std::cout << "Container Networks: " << std::endl;
	for (Labs_Core::Network network : networks) {
		std::cout << "\t" << network.Get_Name_Cache() << std::endl;
	}

	// Test restart and stop commands
	docker.Restart(result);
	docker.Stop(result);

	std::cout << "Checking getting the container with the name" << std::endl;
	if (docker.Get_Container(name).Get_ID() == docker.Get_Container_ID(result)) {
		std::cout << "Success!" << std::endl;
	}
	else {
		std::cout << "Failed" << std::endl;
	}

	// Start, reset, kill and remove the container
	docker.Start(result);
	result = docker.Reset(result);
	docker.Kill(result);
	docker.Remove(result);
	return 0;
}

// Handler for test commands (api, container-control)
int Labs_CLI::Docker::Test_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	static std::map<std::string_view, std::function<int(Labs_CLI::Command_Interpreter, int, char**)>> possible_commands = {
		{"api", Labs_CLI::Docker::Test_API},
		{"container-control", Labs_CLI::Docker::Test_Container_Control}
	};

	auto it = possible_commands.find(command.Get_SubCommand());

	if (it != possible_commands.end()) {
		return it->second(command, argc, argv);
	}
	else {
		std::cerr << "Unknown command: " << command.Get_SubCommand() << std::endl;
		std::cerr << "Run '--help' or '-h' for help finding a command" << std::endl;
		return 1;
	}
	return 1;
}

// Handler for get commands (container, network)
int Labs_CLI::Docker::Get_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	static std::map<std::string_view, std::function<int(Labs_CLI::Command_Interpreter, int, char**)>> possible_commands = {
		{"container", Labs_CLI::Docker::Get_Container_Info},
		{"network", Labs_CLI::Docker::Get_Network_Info}
	};

	auto it = possible_commands.find(command.Get_SubCommand());

	if (it != possible_commands.end()) {
		return it->second(command, argc, argv);
	}
	else {
		std::cerr << "Unknown command: " << command.Get_SubCommand() << std::endl;
		std::cerr << "Run '--help' or '-h' for help finding a command" << std::endl;
		return 1;
	}
	return 1;
}

// Help command for docker CLI usage
int Labs_CLI::Docker::Help(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	std::cout << "./labs-cli docker <command> [<subcommand>] [<flags>]" << std::endl;
	std::cout << std::endl;
	std::cout << "The current available commands are:" << std::endl;
	std::cout << "\thelp - Displays this help message" << std::endl;
	std::cout << "\ttest - group of commands for testing thr program works correctly" << std::endl;
	std::cout << "\t\tapi - Tests that the HTTP API requests for docker are working" << std::endl;
	std::cout << "\t\tcontainer-control - Tests if the container controls are working" << std::endl;
	std::cout << "\tstart - Starts a docker container" << std::endl;
	std::cout << "\tstop - Stops a running docker container" << std::endl;
	std::cout << "\trestart - Restarts the docker container" << std::endl;
	std::cout << "\treset - Resets a container back to its default image" << std::endl;
	std::cout << "\tget - Group of commands for getting data on certain objects" << std::endl;
	std::cout << "\t\tcontainer - Gets data about a specified container" << std::endl;
	std::cout << "\t\tnetwork - Gets data about a specified network" << std::endl;
	std::cout << std::endl;
	std::cout << "Run './labs-cli docker <command> --help' for more info on a command" << std::endl;
	return 0;
}

// Command to start a container by name
int Labs_CLI::Docker::Start(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	Labs_Core::Docker docker = Labs_Core::Docker();
	std::string name = "";

	// Define options to accept container name
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers")
		("name,n", po::value<std::string>(&name)->required(), "Lets you specify the name of the container");

	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (vm.count("help")) {
		std::cout << "Starts the specified container" << std::endl;
		std::cout << "Available flags:" << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	// Start container by name
	Labs_Core::Container result = docker.Get_Container(name);
	docker.Start(result);
	return 0;
}

// Command to stop a container by name
int Labs_CLI::Docker::Stop(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	Labs_Core::Docker docker = Labs_Core::Docker();
	std::string name = "";

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers")
		("name,n", po::value<std::string>(&name)->required(), "Lets you specify the name of the container");

	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (vm.count("help")) {
		std::cout << "Stops the specified container" << std::endl;
		std::cout << "Available flags:" << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	Labs_Core::Container result = docker.Get_Container(name);
	docker.Stop(result);
	return 0;
}int Labs_CLI::Docker::Reset(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	Labs_Core::Docker docker = Labs_Core::Docker(); // Create Docker object to interact with Docker engine
	std::string name = ""; // Variable to hold container name from CLI

	// Define CLI options for this command
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers") // Help flag
		("name,n", po::value<std::string>(&name)->required(), "Lets you specify the name of the container"); // Required container name

	// Parse the CLI arguments
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm); // Parse command line args into variables map
		po::notify(vm); // Throws if required options like "name" are missing
	}
	catch (const po::error& e) {
		// Handle parsing errors by printing the error and usage
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1; // Return error code
	}

	// If user requests help, print help text and exit
	if (vm.count("help")) {
		std::cout << "Resets the specified container back to its default state" << std::endl;
		std::cout << "Available flags:" << std::endl;
		std::cout << desc << std::endl;
		return 0; // Success exit
	}

	// Get the container object by name
	Labs_Core::Container result = docker.Get_Container(name);

	// Reset the container to default state
	docker.Reset(result);
	return 0; // Success exit
}

int Labs_CLI::Docker::Restart(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	Labs_Core::Docker docker = Labs_Core::Docker(); // Create Docker object
	std::string name = ""; // Container name to restart

	// Define CLI options for restart command
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers") // Help flag
		("name,n", po::value<std::string>(&name)->required(), "Lets you specify the name of the container"); // Required container name

	// Parse the command line arguments
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm); // Check required options
	}
	catch (const po::error& e) {
		// Print parsing errors and usage
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	// Print help if requested
	if (vm.count("help")) {
		std::cout << "Restarts the specified container" << std::endl;
		std::cout << "Available flags:" << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	// Get the container object by name
	Labs_Core::Container result = docker.Get_Container(name);

	// Restart the container
	docker.Restart(result);
	return 0;
}

int Labs_CLI::Docker::Get_Container_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	Labs_Core::Docker docker = Labs_Core::Docker(); // Docker object
	std::string name = ""; // Optional container name
	std::string ip = ""; // Optional container IP
	std::string id = ""; // Optional container ID

	// Define allowed CLI options
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers")
		("name,n", po::value<std::string>(&name), "Lets you specify the name of the container")
		("ip,i", po::value<std::string>(&ip), "Lets you specify the exact ip of the container")
		("id,d", po::value<std::string>(&id), "Lets you specify the exact i of the container");

	// Parse the CLI arguments into variables map
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm); // Notify triggers errors for required fields, if any
	}
	catch (const po::error& e) {
		// Print parsing error and usage
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	// If help flag specified, print help info and exit
	if (vm.count("help")) {
		std::cout << "Restarts the specified container" << std::endl;
		std::cout << "Available flags:" << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	// Require at least one identifier (name, ip, or id) to find the container
	if (name == "" && ip == "" && id == "") {
		std::cerr << "You must provide either a name, ip, or an id" << std::endl;
		return 1;
	}

	std::optional<Labs_Core::Container> opt_container;

	if (ip == "") {
		// If IP not specified, get container by name or id
		opt_container = docker.Get_Container(name != "" ? name : id);
	}
	else {
		// Otherwise, find container by IP from all containers
		std::vector<Labs_Core::Container> containers = docker.Get_All_Containers();
		for (Labs_Core::Container con : containers) {
			if (con.Get_IP_Cache() == ip) {
				opt_container = con;
				break;
			}
		}
	}

	// If container not found, print error and exit
	if (!opt_container.has_value()) {
		std::cerr << "No container has that IP " << ip << std::endl;
		return 1;
	}

	// Container found, get its value
	Labs_Core::Container container = opt_container.value();

	// Print container details
	std::string container_id = docker.Get_Container_ID(container);
	std::cout << "Container Id: " << container_id << std::endl;
	std::string container_name = docker.Get_Container_Name(container);
	std::cout << "Container Name: " << container_name << std::endl;
	std::string container_ip = docker.Get_IP(container);
	std::cout << "Container IP: " << container_ip << std::endl;
	std::string image = docker.Get_Image(container);
	std::cout << "Container Image: " << image << std::endl;
	std::string status = docker.Get_Status(container) ? "up" : "down";
	std::cout << "Container Status: " << status << std::endl;

	// List all networks the container is connected to
	std::vector<Labs_Core::Network> networks = docker.Get_Networks(container);
	std::cout << "Container Networks: " << std::endl;
	for (Labs_Core::Network network : networks) {
		std::cout << "\t" << network.Get_Name_Cache() << std::endl;
	}

	return 0; // Successful completion
}



int Labs_CLI::Docker::Get_Network_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	Labs_Core::Docker docker = Labs_Core::Docker(); // Create Docker interface object
	std::string name = "";       // Network name filter
	std::string iprange = "";    // IP range filter
	std::string id = "";         // Network ID filter
	std::string gateway = "";    // Gateway filter
	std::string subnet = "";     // Subnet filter

	// Define CLI options available for this command
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers")
		("name,n", po::value<std::string>(&name), "Lets you specify the name of the container")      // Note: This says "container" but likely means network
		("id,d", po::value<std::string>(&id), "Lets you specify the exact ip of the container")       // Also seems copy-paste leftover — should say "network id"
		("iprange,i", po::value<std::string>(&iprange), "Lets you specify the exact ip of the network")
		("gateway,g", po::value<std::string>(&gateway), "Lets you specify the exact gateway of the network")
		("subnet,s", po::value<std::string>(&subnet), "Lets you specify the exact subnet of the network");

	// Parse the CLI arguments
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm); // Throws if required options are missing (none required here)
	}
	catch (const po::error& e) {
		// Handle parsing errors by printing message and usage info
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	// If user requests help, print help info and exit
	if (vm.count("help")) {
		std::cout << "Displays information about Docker networks." << std::endl;
		std::cout << "Available flags:" << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	// Require at least one filter to find networks
	if (name == "" && iprange == "" && id == "" && gateway == "" && subnet == "") {
		std::cerr << "You must provide either a name, iprange, id, gateway, or subnet" << std::endl;
		return 1;
	}

	std::vector<Labs_Core::Network> networks;

	if (id != "" || name != "") {
		// If ID or name provided, get that specific network
		// If both provided, ID takes priority
		networks.push_back(docker.Get_Network(id != "" ? id : name));
	}
	else {
		// Otherwise, filter all networks based on iprange, subnet, gateway
		std::vector<Labs_Core::Network> all_networks = docker.Get_All_Networks();
		for (Labs_Core::Network network : all_networks) {
			// Match filters, ignoring empty filter strings
			if ((iprange == "" || network.Get_IP_Range_Cache() == iprange) &&
				(subnet == "" || subnet == network.Get_Subnet_Cache()) &&
				(gateway == "" || gateway == network.Get_Gateway_Cache())) {
				networks.push_back(network);
			}
		}

		// If no networks matched filters, print error and exit
		if (networks.size() == 0) {
			std::cerr << "No networks fit those criteria" << std::endl;
			return 1;
		}
	}

	bool looped_yet = false;

	// Print info for each matched network
	for (Labs_Core::Network network : networks) {
		if (looped_yet) {
			std::cout << std::endl; // Separate multiple networks output
		}
		else {
			looped_yet = true;
		}
		std::cout << "Network Id: " << network.Get_ID() << std::endl;
		std::cout << "Network Name: " << network.Get_Name_Cache() << std::endl;
		std::cout << "Network IP Range: " << network.Get_IP_Range_Cache() << std::endl;
		std::cout << "Network Gateway: " << network.Get_Gateway_Cache() << std::endl;
		std::cout << "Network Subnet: " << network.Get_Subnet_Cache() << std::endl;

		// Get all containers connected to this network
		std::vector<Labs_Core::Container> containers = docker.Get_Containers(network);
		std::cout << "Network's Containers: " << std::endl;

		// Print each container name
		for (Labs_Core::Container container : containers) {
			container.Cache_Update(); // Update container info cache before printing
			std::cout << "\t" << container.Get_Name_Cache() << std::endl;
		}
	}

	return 0; // Success
}


int Labs_CLI::Docker::Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	// Map command strings to their handling functions
	static std::map<std::string_view, std::function<int(Labs_CLI::Command_Interpreter, int, char**)>> possible_commands = {
		{"help", Labs_CLI::Docker::Help},
		{"test", Labs_CLI::Docker::Test_Handler},
		{"start", Labs_CLI::Docker::Start},
		{"stop", Labs_CLI::Docker::Stop},
		{"restart", Labs_CLI::Docker::Restart},
		{"reset", Labs_CLI::Docker::Reset},
		{"get", Labs_CLI::Docker::Get_Handler},
	};

	// Define general help option
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Provides help on what flags this command offers");

	// Try to find the requested command in the command map
	auto it = possible_commands.find(command.Get_Command());

	if (it != possible_commands.end()) {
		// If found, call the appropriate handler function and return its result
		return it->second(command, argc, argv);
	}
	else if (command.Get_Command() == "") {
		// No command specified — maybe user wants general help?

		po::variables_map vm;
		try {
			// Parse only help flag here
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm);
		}
		catch (const po::error& e) {
			// Print error and usage if parsing fails
			std::cerr << "Error: " << e.what() << std::endl;
			std::cerr << desc << std::endl;
			return 1;
		}

		if (vm.count("help")) {
			// Print general help info
			std::cout << "Available flags:" << std::endl;
			std::cout << desc << std::endl;
			Labs_CLI::Docker::Help(command, argc, argv); // Call help handler for detailed help
			return 0;
		}
	}
	else {
		// Command was specified but not recognized — print error
		std::cerr << "Unknown command: " << command.Get_Command() << std::endl;
		std::cerr << "Run '--help' or '-h' for help finding a command" << std::endl;
		return 1;
	}

	return 1; // Default fallback return error
}
