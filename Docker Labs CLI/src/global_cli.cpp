#include <random>
#include <fstream>
#include <boost/program_options.hpp>
#include "docker_labs/cli/global_cli.h"
#include "docker_labs/cli/cloudflare_cli.h"

using namespace Docker_Labs;
namespace po = boost::program_options;

int Labs_CLI::Global_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
    if (command.Get_Partition() == "add") {
        return Init_Handler(argc, argv);
    }
    else if (command.Get_Partition() == "rm") {
        return Rm_Handler(argc, argv);
    }
    else if (command.Get_Partition() == "nuke") {
        return Nuke(argc, argv);
    }
    else if (command.Get_Partition() == "instantiate") {
    	return Instantiate(argc, argv);
    }
    else {
        // Define the options
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Show help message");

        // Parse the command line arguments
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
            std::cout << "A command used to create a container and setup the cloudflare rules for it in one." << std::endl;
            std::cout << desc << std::endl;
            return 0;
        }
    }
    return 1;
}

int Labs_CLI::Instantiate(int argc, char* argv[]) {
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth, false);
    Labs_Core::Docker docker = Labs_Core::Docker();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 35);

    std::string filename;
    std::string image;
    std::string container_name;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("file,u", po::value<std::string>(&filename)->required(), "Provide a file with an email on each line that define the default emails for each of the containers")
        ("image,i", po::value<std::string>(&image)->required(), "Provide the image that the container should be created from");

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
        std::cout << "A command used to create a container and setup the cloudflare rules for it in one." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    static const std::string chars =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";

    if (filename.empty()){
        std::cerr << "You must specify the filename." << std::endl;
        return 1;
    }
    if (image.empty()){
        std::cerr << "You must specify an image" << std::endl;
    	return 1;
    }

    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return 1;
    }


    std::string email;

    while (std::getline(file, email)) {
	container_name = "";
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
    


        Labs_Core::Container container = docker.Create_Container(container_name, image);
   
        cloudflare.Activate_Container(container, Labs_Core::User(email));
    }

    file.close();
    return 0;
}

int Labs_CLI::Init_Handler(int argc, char* argv[])
{
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth, false);
    Labs_Core::Docker docker = Labs_Core::Docker();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 35);

    std::string email;
    std::string image;
    std::string container_name;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("user,u", po::value<std::string>(&email)->required(), "Provide an email that the container by default allows access to")
        ("image,i", po::value<std::string>(&image)->required(), "Provide the image that the container should be created from")
        ("name,n", po::value<std::string>(&container_name), "Set a custom name for a container");

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
        std::cout << "A command used to create a container and setup the cloudflare rules for it in one." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    static const std::string chars =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";

    if (email.empty()){
        std::cerr << "You must specify the user email." << std::endl;
        return 1;
    }
    if (image.empty()){
        std::cerr << "You must specify an image" << std::endl;
    	return 1;
    }

    if (container_name == "") {
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


    Labs_Core::Container container = docker.Create_Container(container_name, image);
   
    cloudflare.Activate_Container(container, Labs_Core::User(email));
    return 0;
}

int Labs_CLI::Rm_Handler(int argc, char* argv[])
{
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth, false);
    Labs_Core::Docker docker = Labs_Core::Docker();
    
    bool keep_container;
    std::string container_name;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("name,n", po::value<std::string>(&container_name), "The name of the container to be removed from cloudflare.")
        ("keep-container,k", po::bool_switch(&keep_container), "If the local docker container should be removed.");

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
        std::cout << "A command used to create a container and setup the cloudflare rules for it in one." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    Labs_Core::Container container = docker.Get_Container(container_name);
    cloudflare.Deactivate_Container(container, keep_container);
    return 0;
}

int Labs_CLI::Nuke(int argc, char* argv[])
{
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth, false);
    Labs_Core::Docker docker = Labs_Core::Docker();

    bool keep_containers;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("keep-container,k", po::bool_switch(&keep_containers), "If the local docker containers should be removed.");

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
        std::cout << "A command used to create a container and setup the cloudflare rules for it in one." << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    std::vector<Labs_Core::Container> containers = docker.Get_All_Containers();

    bool err = false;

    for (Labs_Core::Container container : containers) {
        err = err + (bool)cloudflare.Deactivate_Container(container, keep_containers);
    }

    return err;
}
