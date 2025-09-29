/**
 * cloudflare_cli.cpp
 * Implementation of Cloudflare CLI command handlers.
 *
 * Implements the logic for authenticating with Cloudflare and
 * handling CLI commands specific to Cloudflare functionality.
 */

#include <vector>
#include "docker_labs/cli/cloudflare_cli.h"
#include "docker_labs/cli/cloudflare_cli_tests.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"
#include "boost/program_options.hpp"
#ifdef _WIN32
#include <io.h>
#define isPiped() !_isatty(_fileno(stdin))
#else
#include <unistd.h>
#define isPiped() !isatty(fileno(stdin))
#endif

using namespace Docker_Labs;
namespace po = boost::program_options;

// === Modern Auth Handler ===
	// Reads a connection string from stdin and parses it using Cloudflare's API_Auth factory.
Labs_Core::Cloudflare::API_Auth Labs_CLI::Cloudflare::Get_Auth() {
	std::string conn_str;

	try {
		// First, try to get the connection string from the environment
		return Labs_Core::Cloudflare::API_Auth::From_Env();
	}
	catch (const std::runtime_error&) {
		std::cout << "Failed to retrieve connection string from environment." << std::endl;
	}

	

	try {
		// If that fails, read it from piped stdin// If that fails, read from standard input
		std::getline(std::cin, conn_str);  // Use getline to support spaces

		if (conn_str.empty()) {
			throw std::runtime_error("Connection string is empty.");
		}
		auto cf_auth = Labs_Core::Cloudflare::API_Auth::From_Connection_String(conn_str);
		return cf_auth;
	}
	catch (const std::exception& e) {
		std::cout << "Error reading connection string: " << e.what() << std::endl;
		std::exit(1);
	}
}


// === Legacy Auth Handler ===
// Reads Cloudflare credentials from piped stdin.
Labs_Core::Cloudflare::API_Auth Labs_CLI::Cloudflare::Get_Auth_Legacy() {
	// Reject if input is not piped
	if (!isPiped()) {
		std::cerr << "Error: Cloudflare legacy keys must be piped into the program." << std::endl;
		std::exit(1);
	}

	std::string account_id;
	std::string zone_id;
	std::string tunnel_id;
	std::string token;
	std::string domain;

	// Read legacy values in a fixed order
	std::getline(std::cin, account_id);
	std::getline(std::cin, zone_id);
	std::getline(std::cin, tunnel_id);
	std::getline(std::cin, token);
	std::getline(std::cin, domain);

	auto cf_auth = Labs_Core::Cloudflare::API_Auth(
		account_id,
		zone_id,
		tunnel_id,
		token,
		domain
	);

	return cf_auth;
}

int Labs_CLI::Cloudflare::Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
	std::string command_str = command.Get_Command();
	std::string subcommand_str = command.Get_SubCommand();

	// === Special Command: Convert legacy keys into a connection string ===
	if (command_str == "create_conn_str") {
		Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth_Legacy();
		std::cout << cf_auth.Generate_Connection_String() << std::endl;
		return 0; // Use return instead of exit for better control in testing
	}

	// === Load Auth Credentials ===
	const auto cf_auth = Labs_CLI::Cloudflare::Get_Auth();

	// === Command Maps ===

	static const std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>> fetch_commands = {
		{"seats",   Fetch_Seats},
		{"ingress", Fetch_Ingress},
		{"dns",     Fetch_DNS_Records}
	};

	static const std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>> test_commands = {
		{"api",         Test_API},
		{"ingress",     Test_Ingress},
		{"dns",         Test_DNS},
		{"app",         Test_Application},
		{"application", Test_Application},
		{"full-init",   Test_Initialize},
		{"grant",       Test_Grant_Policy}
	};

	static const std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>> create_commands = {
		{"ingress",     Create_Ingress},
		{"dns",         Create_DNS},
		{"app",         Create_Application},
		{"application", Create_Application}
	};

	static const std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>> remove_commands = {
		{"ingress",     Remove_Ingress},
		{"dns",         Remove_DNS},
		{"app",         Remove_Application},
		{"application", Remove_Application}
	};

	// === Subcommand Dispatchers ===

	auto try_dispatch = [&](const auto& map, auto&&... args) -> std::optional<int> {
		const auto it = map.find(subcommand_str);
		if (it != map.end()) {
			return it->second(std::forward<decltype(args)>(args)...);
		}
		return std::nullopt;
		};

	if (command_str == "fetch") {
		if (std::optional<int> result = try_dispatch(fetch_commands, cf_auth)) return *result;
	}
	else if (command_str == "test") {
		if (std::optional<int> result = try_dispatch(test_commands, cf_auth)) return *result;
	}
	else if (command_str == "create") {
		if (std::optional<int> result = try_dispatch(create_commands, cf_auth, argc, argv)) return *result;
	}
	else if (command_str == "remove") {
		if (std::optional<int> result = try_dispatch(remove_commands, cf_auth, argc, argv)) return *result;
	}
	else if (command_str == "update") {
		return Update_Ingress(cf_auth, argc, argv);
	}
	else if (command_str == "grant") {
		return Grant_Container(cf_auth, argc, argv);
	}
	else if (command_str == "revoke") {
		return Revoke_Container(cf_auth, argc, argv);
	}
	else if (command_str == "deactivate") {
		return Deactivate_Seats(cf_auth, argc, argv);
	}

	// === Unknown Command or Missing Subcommand ===
	return Help_Message(argc, argv);
}


int Labs_CLI::Cloudflare::Fetch_Seats(Labs_Core::Cloudflare::API_Auth cf_auth)
{
	// Fetch all user seats from Cloudflare API
	std::vector<Labs_Core::User_Seat> users = Labs_Core::Cloudflare(cf_auth).Fetch_Seats();

	for (Labs_Core::User_Seat& user : users) {
		std::cout << "Seat UID: " << user.Get_SeatUID() << '\n';
		std::cout << "Name: " << user.Get_Name() << '\n';
		std::cout << "Email: " << user.Get_Email() << '\n';
		std::cout << "Created: " << user.Get_CreatedAt() << '\n';
		std::cout << "Last Accessed: " << user.Get_LastSuccessfulLogin() << '\n';

		std::cout << "Seats: "
			<< (user.Get_AccessSeat() ? "Access" : "")
			<< (user.Get_AccessSeat() && user.Get_GatewaySeat() ? ", " : "")
			<< (user.Get_GatewaySeat() ? "Gateway" : "")
			<< (!user.Get_AccessSeat() && !user.Get_GatewaySeat() ? "None" : "")
			<< "\n\n";
	}

	return 0;
}


int Labs_CLI::Cloudflare::Fetch_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth)
{
	// Fetch ingress configuration JSON from Cloudflare API
	json response_body = Labs_Core::Cloudflare(cf_auth).Fetch_Ingress();

	if (!response_body.value("success", false)) {
		std::cerr << "Failed to fetch ingress configuration." << std::endl;
		return 1;
	}

	json ingress_conf = response_body["result"]["config"]["ingress"];
	if (!ingress_conf.empty()) {
		ingress_conf.erase(ingress_conf.end() - 1); // Remove last element (default or catch-all)
	}

	for (const auto& domain : ingress_conf) {
		std::cout << "Hostname: " << domain.value("hostname", "<none>") << '\n';
		std::cout << "Service: " << domain.value("service", "<none>") << "\n\n";
	}

	return 0;
}


int Labs_CLI::Cloudflare::Fetch_DNS_Records(Labs_Core::Cloudflare::API_Auth cf_auth)
{
	// Fetch DNS records JSON from Cloudflare API
	json response_body = Labs_Core::Cloudflare(cf_auth).Fetch_DNS_Records();

	if (!response_body.value("success", false)) {
		std::cerr << "Failed to fetch DNS records." << std::endl;
		return 1;
	}

	json records = response_body.value("result", json::array());
	std::cout << "Total Records: " << records.size() << "\n\n";
	for (json record : records) {
		// Parse comment to extract container and owner info
		std::cout << "Record ID: " << record.value("id", "<none>") << '\n';
		std::cout << "Name: " << record.value("name", "<none>") << '\n';
		std::cout << "Proxy: " << record.value("content", "<none>") << "\n\n";
	}

	return 0;
}


Labs_Core::Container Labs_CLI::Cloudflare::Spec_Container(int argc, char* argv[])
{
	Labs_Core::Docker docker = Labs_Core::Docker();

	std::string container_name;
	// Define the options
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Shows this popup")
		("container,c", po::value<std::string>(&container_name)->required(), "Specifies the name of the container");

	// Parse the command line arguments
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm); // Throws if required options are missing
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << desc << std::endl;
		exit(1);
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(0);
	}

	Labs_Core::Container cont = docker.Get_Container(container_name);
	cont.Cache_Update();
	return cont;
}
std::vector<Labs_Core::User> Labs_CLI::Cloudflare::Spec_User(int argc, char* argv[])
{
	std::vector<std::string> emails;
	std::vector<Labs_Core::User> users;

	// Define command-line options for specifying users by email
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Shows this help message")
		("user,u", po::value<std::vector<std::string>>(&emails)->multitoken(), "Specify user email(s)");

	// Parse command line arguments
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << '\n' << desc << std::endl;
		exit(1);
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(0);
	}

	if (!emails.empty()) {
		for (const auto& email : emails) {
			users.emplace_back(email);
		}
		return users;
	}

	// If no valid options were specified, print help and exit
	std::cout << desc << std::endl;
	exit(1);
}


std::vector<Labs_Core::User_Seat> Labs_CLI::Cloudflare::Spec_Seat(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	Labs_Core::Cloudflare cloudflare(cf_auth);
	std::vector<std::string> emails;
	std::vector<Labs_Core::User_Seat> seated_users;

	// Define command-line options for selecting user seats
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Shows this help message")
		("user,u", po::value<std::vector<std::string>>(&emails)->multitoken(), "Specify user email(s)")
		("all,a", "Select all seated users");

	// Parse command line arguments
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << '\n' << desc << std::endl;
		exit(1);
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(0);
	}

	if (vm.count("all")) {
		// Return all seats
		return cloudflare.Fetch_Seats();
	}

	if (!emails.empty()) {
		for (const auto& email : emails) {
			seated_users.push_back(cloudflare.Fetch_Seat(Labs_Core::User(email)));
		}
		return seated_users;
	}

	// If no valid options specified, show help and exit
	std::cout << desc << std::endl;
	exit(1);
}


std::tuple<Labs_Core::Container, Labs_Core::User> Labs_CLI::Cloudflare::Spec_Container_User(int argc, char* argv[])
{
	Labs_Core::Docker docker;
	std::string container_name;
	std::string user_email;

	// Define command-line options for container and user
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Shows this help message")
		("container,c", po::value<std::string>(&container_name)->required(), "Specify container name")
		("user,u", po::value<std::string>(&user_email)->required(), "Specify user email");

	// Parse command line arguments
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error& e) {
		std::cerr << "Error: " << e.what() << '\n' << desc << std::endl;
		exit(1);
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(0);
	}

	Labs_Core::Container container = docker.Get_Container(container_name);
	container.Cache_Update();

	Labs_Core::User user(user_email);

	return std::make_tuple(container, user);
}
// Creates a Cloudflare ingress configuration for a specified container
int Labs_CLI::Cloudflare::Create_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	// Parse container details from command line arguments
	Labs_Core::Container container = Spec_Container(argc, argv);

	// Call Cloudflare API to create ingress config for the container
	return Labs_Core::Cloudflare(cf_auth).Create_Ingress(container);
}

// Updates an existing Cloudflare ingress configuration for a specified container
int Labs_CLI::Cloudflare::Update_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	Labs_Core::Container container = Spec_Container(argc, argv);
	return Labs_Core::Cloudflare(cf_auth).Update_Ingress(container);
}

// Removes a Cloudflare ingress configuration for a specified container
int Labs_CLI::Cloudflare::Remove_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	Labs_Core::Container container = Spec_Container(argc, argv);
	return Labs_Core::Cloudflare(cf_auth).Remove_Ingress(container);
}

// Creates a DNS record in Cloudflare for the specified container
int Labs_CLI::Cloudflare::Create_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	Labs_Core::Container container = Spec_Container(argc, argv);
	return Labs_Core::Cloudflare(cf_auth).Create_DNS_Record(container);
}

// Removes a DNS record in Cloudflare for the specified container
int Labs_CLI::Cloudflare::Remove_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	Labs_Core::Container container = Spec_Container(argc, argv);
	return Labs_Core::Cloudflare(cf_auth).Remove_DNS_Record(container);
}

// Creates an application in Cloudflare for the specified container
int Labs_CLI::Cloudflare::Create_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	Labs_Core::Container container = Spec_Container(argc, argv);
	return Labs_Core::Cloudflare(cf_auth).Create_Application(container);
}

// Removes an application from Cloudflare for the specified container
int Labs_CLI::Cloudflare::Remove_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	Labs_Core::Container container = Spec_Container(argc, argv);
	return Labs_Core::Cloudflare(cf_auth).Remove_Application(container);
}

// Grants a user access to a specified container in Cloudflare
int Labs_CLI::Cloudflare::Grant_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	// Parse container and user details from command line arguments
	auto [container, user] = Spec_Container_User(argc, argv);

	// Call Cloudflare API to grant access
	return Labs_Core::Cloudflare(cf_auth).Grant_Container(container, user);
}

// Revokes a user's access to a specified container in Cloudflare
int Labs_CLI::Cloudflare::Revoke_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	auto [container, user] = Spec_Container_User(argc, argv);
	return Labs_Core::Cloudflare(cf_auth).Revoke_Container(container, user);
}

// Deactivates user seats in Cloudflare based on provided arguments
int Labs_CLI::Cloudflare::Deactivate_Seats(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
	// Get list of user seats based on command-line flags (--user, --all)
	std::vector<Labs_Core::User_Seat> seated_users = Spec_Seat(cf_auth, argc, argv);

	// Create Cloudflare API wrapper instance
	Labs_Core::Cloudflare cloudflare(cf_auth);

	// Loop over each seat and deactivate it
	for (const Labs_Core::User_Seat& seat : seated_users) {
		cloudflare.Deactivate_Seat(seat);
	}

	return 0;
}

// Prints help message describing the Cloudflare CLI commands and options
int Labs_CLI::Cloudflare::Help_Message(int argc, char* argv[])
{
	std::cout << "INSERT HELP MESSAGE HERE" << std::endl;
	return 0;
}
