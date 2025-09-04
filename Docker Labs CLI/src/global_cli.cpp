#include "global_cli.h"
#include "labs_container.h"
#include "docker_hook.h"
#include "labs_user.h"
#include "cloudflare_hook.h"
#include "main.h"
#include <string>
#include <iostream>
#include <getopt.h>
#include <random>

int Docker_Labs::Labs_CLI::Global_Handler(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
    Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
    Docker_Labs::Cloudflare::API_Auth cf_auth = Cloudflare::API_Auth::Get_Auth();
    Docker_Labs::Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth, false);
    bool keep_container = false;

    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"user", required_argument, nullptr, 'u'},
        {"image", required_argument, nullptr, 'i'},
        {"name", required_argument, nullptr, 'n'},
        {"keep", no_argument, nullptr, 'n'},
        { nullptr, 0, nullptr, 0 }
    };

    std::string email = "";
    std::string image = "";
    std::string container_name = "";

    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "hu:i:n:k", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "A command used to create a container and setup the cloudflare rules for it in one." << std::endl;
            std::cout << "\t-h, --help: used to show this (hopfully) helpful popup about how to use this command" << std::endl;
            std::cout << "\t-u --user: [Required] provide an email that the container by default allows access to" << std::endl;
            std::cout << "\t-i --image: [Required] provide the image that the container should be created from" << std::endl;
            std::cout << "\t-n, --name: used to set a custome name for a container" << std::endl;
            // RM stuff
            std::cout << "A command used to stop a container and remove the cloudflare rules for it in one." << std::endl;
            std::cout << "\t-h, --help: used to show this (hopfully) helpful popup about how to use this command" << std::endl;
            std::cout << "\t-n, --name: [Required] the name of the container you want to be removed" << std::endl;
            std::cout << "\t-k, --keep-container: don't delete the container, but do delete all the cloudflare rules" << std::endl;
            // Nuke suff
            std::cout << "A command used to remove all containers and remove all the cloudflare rules for them in one." << std::endl;
            std::cout << "\t-h, --help: used to show this (hopfully) helpful popup about how to use this command" << std::endl;
            std::cout << "\t-k, --keep-containers: Only stop the containers not kill them" << std::endl;
            return 0;
        case 'u':
            email = optarg;
            continue;
        case 'i':
            image = optarg;
            continue;
        case 'n':
            container_name = optarg;
            continue;
        case 'k':
            keep_container = true;
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

    if (command.Get_Partition() == "add") {

        if (container_name == "") {
            return Init_Handler(cloudflared, docker, email, image);
        }
        else {
            return Init_Handler(cloudflared, docker, email, image, container_name);
        }
    }
    else if (command.Get_Partition() == "rm") {
        return Rm_Handler(cloudflared, docker, container_name, keep_container);
    }
    else if (command.Get_Partition() == "nuke") {
        return Nuke(cloudflared, docker, keep_container);
    }
    return 1;
}

int Docker_Labs::Labs_CLI::Init_Handler(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, std::string email, std::string image) {
    return Init_Handler(cloudflared, docker, email, image, "");
}

int Docker_Labs::Labs_CLI::Init_Handler(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, std::string email, std::string image, std::string container_name)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 35);
    static const std::string chars =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";

    if (email == ""){
    	std::cerr << "You must provide an email";
    	return 1;
    }
    if (image == ""){
    	std::cerr << "You must provide an image";
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


    Docker_Labs::Container container = docker.Create_Container(container_name, image);
   
    cloudflared.Init_Access(container, User(email));
    return 0;
}

int Docker_Labs::Labs_CLI::Rm_Handler(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, std::string container_name, bool keep_container)
{
    std::cout << container_name << std::endl;
    Docker_Labs::Container container = docker.Get_Container(container_name);
    container.Cache_Update();
    cloudflared.Remove_Application(container);
    cloudflared.Remove_DNS_Record(container);
    cloudflared.Remove_Ingress(container);

    if (keep_container == false) {
        docker.Remove(container);
    }
    else {
        docker.Stop(container);
    }

    return 0;
}

int Docker_Labs::Labs_CLI::Nuke(Docker_Labs::Cloudflare::Cloudflared cloudflared, Docker_Labs::Docker::Docker docker, bool keep_containers)
{


    std::vector<Docker_Labs::Container> containers = docker.Get_All_Containers();

    bool err = false;

    for (Docker_Labs::Container container : containers) {
        err += (bool)Docker_Labs::Labs_CLI::Rm_Handler(cloudflared, docker, container.Get_Name_Cache(), keep_containers);
    }

    return err;
}