#include "container_rm.h"
#include "labs_container.h"
#include "docker_hook.h"
#include "labs_user.h"
#include "cloudflare_hook.h"
#include <string>
#include <iostream>
#include <getopt.h>


int Docker_Labs::Rm_Handler(int argc, char* argv[])
{
    Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
    
    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"name", required_argument, nullptr, 'n'},
        {"keep-container", no_argument, nullptr , 'k'},
        { nullptr, 0, nullptr, 0 }
    };

    std::string container_name = "";
    bool keep_container = false;

    int opt;
    while ((opt = getopt_long(argc, argv, "hn:k", long_flags, nullptr)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "A command used to stop a container and remove the cloudflare rules for it in one." << std::endl;
            std::cout << "\t-h, --help: used to show this (hopfully) helpful popup about how to use this command" << std::endl;
            std::cout << "\t-n, --name: [Required] the name of the container you want to be removed" << std::endl;
            std::cout << "\t-k, --keep-container: don't delete the container, but do delete all the cloudflare rules" << std::endl;
            return 0;
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

    if (container_name == ""){
    	std::cerr << "You need to specify a container name" << std::endl;
    	return 1;
    }


    Docker_Labs::Container container = docker.Get_Container(container_name);
    container.Cache_Update();
    Cloudflare::API_Auth cf_auth = Cloudflare::API_Auth::Get_Auth();
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth, false);
    cloudflared.Remove_Application(container);
    cloudflared.Remove_DNS_Record(container);
    cloudflared.Remove_Ingress(container);
    
    if(keep_container == false){
    	docker.Remove(container);
    } else {
    	docker.Stop(container);
    }
    
    return 0;
}
