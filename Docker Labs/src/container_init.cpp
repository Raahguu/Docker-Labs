#include "container_init.h"
#include "labs_container.h"
#include "docker_hook.h"
#include "labs_user.h"
#include "cloudflare_hook.h"
#include <string>
#include <iostream>
#include <getopt.h>
#include <random>


int Docker_Labs::Init_Handler(int argc, char* argv[])
{
    Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 35);
    static const std::string chars =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";

    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"user", required_argument, nullptr, 'u'},
        {"image", required_argument, nullptr, 'i'},
        {"name", required_argument, nullptr, 'n'},
        { nullptr, 0, nullptr, 0 }
    };

    std::string email = "";
    std::string image = "";
    std::string container_name = "";

    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "hu:i:n:", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "A command used to create a container and setup the cloudflare rules for it in one." << std::endl;
            std::cout << "\t-h, --help: used to show this (hopfully) helpful popup about how to use this command" << std::endl;
            std::cout << "\t-u --user: [Required] provide an email that the container by default allows access to" << std::endl;
            std::cout << "\t-i --image: [Required] provide the image that the container should be created from" << std::endl;
            std::cout << "\t-n, --name: used to set a custome name for a container" << std::endl;
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
    Cloudflare::API_Auth cf_auth = Cloudflare::API_Auth::Get_Auth();
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth, false);
    cloudflared.Init_Access(container, User(email));
    return 0;
}
