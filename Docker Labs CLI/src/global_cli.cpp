/**
 * global_cli.cpp
 * Implementation of global CLI command handlers.
 *
 * Implements command dispatching for global commands such as add,
 * remove, instantiate, and nuke, integrating Docker and Cloudflare functionality.
 */

#include <random>
#include <fstream>
#include <boost/program_options.hpp>
#include "docker_labs/cli/global_cli.h"
#include "docker_labs/cli/cloudflare_cli.h"

using namespace Docker_Labs;
namespace po = boost::program_options;

// ---------------------------------------------
// Global command handler entry point
// Routes commands to specific handlers based on partition/subcommand
// ---------------------------------------------
int Labs_CLI::Global_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
    if (command.Get_Partition() == "add") {
        return Init_Container_Handler(argc, argv);
    }
    else if (command.Get_Partition() == "rm") {
        return Remove_Container_Handler(argc, argv);
    }
    else if (command.Get_Partition() == "nuke") {
        return Nuke_Cloudflare_Handler(argc, argv);
    }
    else if (command.Get_Partition() == "instantiate") {
        return Bulk_Instantiate_Handler(argc, argv);
    }
    else {
        // Unknown or missing partition: show help message
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Show help message");

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
            std::cout << "A command used to create a container and setup the Cloudflare rules for it in one." << std::endl;
            std::cout << desc << std::endl;
            return 0;
        }
    }

    return 1; // No valid subcommand specified
}

// ---------------------------------------------
// Bulk instantiate containers from a file containing emails
// Creates one container per email and activates Cloudflare rules
// ---------------------------------------------
int Labs_CLI::Bulk_Instantiate_Handler(int argc, char* argv[]) {
    // Initialize Cloudflare and Docker APIs
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare(cf_auth, false);
    Labs_Core::Docker docker;
	bool internal_net = false;

    // Random generator for container name suffix
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 35);

    // Command line option variables
    std::string filename;
    std::string image;
    std::string container_name;

    // Define CLI options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("file,u", po::value<std::string>(&filename)->required(), "File containing an email on each line")
        ("image,i", po::value<std::string>(&image)->required(), "Docker image for container creation")
        ("internal", "Make the container's network internal (no internet)");

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
        std::cout << "Bulk instantiate containers and setup Cloudflare rules." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("internal")) {
        internal_net = true;
    }

    // Allowed characters for random suffix generation
    static const std::string chars = "0123456789abcdefghijklmnopqrstuvwxyz";

    // Validate required inputs
    if (filename.empty()) {
        std::cerr << "You must specify the filename." << std::endl;
        return 1;
    }
    if (image.empty()) {
        std::cerr << "You must specify an image." << std::endl;
        return 1;
    }

    // Open the file containing emails
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return 1;
    }

    // Process each email line
    std::string email;
    while (std::getline(file, email)) {
        container_name.clear();

        // Generate a container name from email:
        // replace non-alphanumeric chars with '_' and convert to lowercase
        for (char c : email) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                container_name.push_back(std::tolower(static_cast<unsigned char>(c)));
            }
            else {
                container_name.push_back('_');
            }
        }
        container_name.push_back('_');

        // Append 3 random characters from allowed charset
        for (int i = 0; i < 3; ++i) {
            container_name.push_back(chars[dist(gen)]);
        }

        // Create container and activate Cloudflare rules for the user
        Labs_Core::Container container = docker.Create_Container(container_name, image, internal_net);
        cloudflare.Activate_Container(container, Labs_Core::User(email));
    }

    file.close();
    return 0;
}

// ---------------------------------------------
// Create a single container and setup Cloudflare rules for one user email
// ---------------------------------------------
int Labs_CLI::Init_Container_Handler(int argc, char* argv[]) {
    // Initialize Cloudflare and Docker APIs
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare(cf_auth, false);
    Labs_Core::Docker docker;
	bool internal_net = false;

    // Random generator for container name suffix
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 35);

    // Command line option variables
    std::string email;
    std::string image;
    std::string container_name;

    // Define CLI options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("user,u", po::value<std::string>(&email)->required(), "Email allowed access to the container")
        ("image,i", po::value<std::string>(&image)->required(), "Docker image for container creation")
        ("name,n", po::value<std::string>(&container_name), "Custom container name (optional)")
        ("internal", "Make the container's network internal (no internet)");

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
        std::cout << "Create a container and setup Cloudflare rules." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("internal")) {
        internal_net = true;
    }

    // Allowed characters for random suffix
    static const std::string chars = "0123456789abcdefghijklmnopqrstuvwxyz";

    // Validate required inputs
    if (email.empty()) {
        std::cerr << "You must specify the user email." << std::endl;
        return 1;
    }
    if (image.empty()) {
        std::cerr << "You must specify an image." << std::endl;
        return 1;
    }

    // If no custom container name provided, generate one from email + random suffix
    if (container_name.empty()) {
        for (char c : email) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                container_name.push_back(std::tolower(static_cast<unsigned char>(c)));
            }
            else {
                container_name.push_back('_');
            }
        }
        container_name.push_back('_');
        for (int i = 0; i < 3; ++i) {
            container_name.push_back(chars[dist(gen)]);
        }
    }

    // Create container and activate Cloudflare for the specified user
    Labs_Core::Container container = docker.Create_Container(container_name, image, internal_net);
    cloudflare.Activate_Container(container, Labs_Core::User(email));
    return 0;
}

// ---------------------------------------------
// Remove a container and its Cloudflare rules
// Optionally keep the local Docker container
// ---------------------------------------------
int Labs_CLI::Remove_Container_Handler(int argc, char* argv[]) {
    // Initialize Cloudflare and Docker APIs
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare(cf_auth, false);
    Labs_Core::Docker docker;

    bool keep_container = false;
    std::string container_name;

    // Define CLI options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("name,n", po::value<std::string>(&container_name), "Name of the container to remove from Cloudflare")
        ("keep-container,k", po::bool_switch(&keep_container), "If true, do not remove the local Docker container");

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
        std::cout << "Remove a container and its Cloudflare rules." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    // Retrieve container object and deactivate Cloudflare rules
    Labs_Core::Container container = docker.Get_Container(container_name);
    cloudflare.Deactivate_Container(container, keep_container);
    return 0;
}

// ---------------------------------------------
// Remove all Cloudflare rules from all containers
// Optionally keep local Docker containers intact
// ---------------------------------------------
int Labs_CLI::Nuke_Cloudflare_Handler(int argc, char* argv[]) {
    // Initialize Cloudflare and Docker APIs
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare(cf_auth, false);
    Labs_Core::Docker docker;

    bool keep_containers = false;

    // Define CLI options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("keep-container,k", po::bool_switch(&keep_containers), "If true, do not remove local Docker containers");

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
        std::cout << "Remove all Cloudflare rules for all containers." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    // Retrieve all containers
    std::vector<Labs_Core::Container> containers = docker.Get_All_Containers();

    bool err = false;
    // Deactivate each container in Cloudflare, track errors
    for (const Labs_Core::Container& container : containers) {
        err = err || static_cast<bool>(cloudflare.Deactivate_Container(container, keep_containers));
    }

    return err;
}
