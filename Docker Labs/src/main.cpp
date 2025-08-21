// Docker Labs.cpp : Defines the entry point for the application.
//

#include "main.h"
#include "cloudflare_hook.h"
#include "labs_user.h"
#include <iostream>
#include <string_view>


int main(int argc, char* argv[])
{   
    using namespace std::literals;
    using namespace Docker_Labs;

    Docker_Labs::Command_Interpreter command = Docker_Labs::Command_Interpreter(argc, argv);
    
    if (true){//command.Get_Partition() == "cloudflare"sv) {

        std::string ACC;
        std::string ZONE;
        std::string TUNN;
        std::string TKN;
        std::string DOMN;

        std::cin >> ACC;
        std::cin >> ZONE;
        std::cin >> TUNN;
        std::cin >> TKN;
        std::cin >> DOMN;

        Docker_Labs::Cloudflare::API_Auth cf_auth = Docker_Labs::Cloudflare::API_Auth(
            ACC,
            ZONE,
            TUNN,
            TKN,
            DOMN
        );

        if (command.Get_Command() == "test-api"sv)
        {
            int responce_code = Docker_Labs::Cloudflare::Test_API(cf_auth);
            switch (responce_code)
            {
            case 0:
                std::cout << "Pass" << std::endl;
                return 0;
            case 2:
                std::cout << "Authorization Failure" << std::endl;
                return 1;
            case 3:
                std::cout << "Connection Failure" << std::endl;
                return 1;
            default:
                std::cout << "Unknown Failure" << std::endl;
                return 1;

            }
        }
        else if (command.Get_Command() == "get-seats"sv) {
            std::vector<Docker_Labs::User> users = Docker_Labs::Cloudflare::Get_Seats(cf_auth);
            for (Docker_Labs::User user : users) {
                std::cout << user.Get_Email() << " " << user.Get_CF_UID() << std::endl;
            }
        }
        else if (command.Get_Command() == "fetch-ingress"sv) {
            json responce_body = Docker_Labs::Cloudflare::Fetch_Ingress(cf_auth);
            if (not responce_body["success"]) {
                std::cout << "Failed to fetch configuration." << std::endl;
                return 1;
            }
            json ingress_conf = responce_body["result"]["config"]["ingress"];
            ingress_conf.erase(ingress_conf.end() - 1);

            for (const json domain : ingress_conf) {
                std::cout << "Hostname: " << domain["hostname"] << std::endl;
                std::cout << "Service: " << domain["service"] << std::endl << std::endl;
            }
        }
        else if (true){//command.Get_Command() == "test_ingress") {
            Container container = Container("docker_id", "test_container_domain_com_e2d", "image", "127.0.0.1", "net_id");
            Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth);
            cloudflared.Create_Ingress(container);
        }
    }
    
    // Use this section for all docker commands
    // e.g. ./labs-cli docker <command> [<subcommand>]
    if (command.Get_Partition() == "docker"sv) {
        /*User user;
        user.email = "rl";
        std::string image = "alpine";
        Docker_Labs::Docker::Create(user, image);
        return 0;*/
    }

}


namespace Docker_Labs {


    Command_Interpreter::Command_Interpreter(int argc, char* argv[]) {

        if (argc < 2) {
            std::cerr << "Usage: labs-cli <partition> [command] [subcommand]\n";
            
        }
        else {

            partition = argv[1];

            int index = 2;

            // Detect first subcommand if it exists and does not start with '-'
            if (argc > index && argv[index][0] != '-') {
                command = argv[index];
                ++index;
            }

            // Detect second subcommand if it exists and does not start with '-'
            if (argc > index && argv[index][0] != '-') {
                subcommand = argv[index];
                ++index;
            }
        }
    }

    std::string Command_Interpreter::Get_Partition()
    {
        return partition;
    }

    std::string Command_Interpreter::Get_Command()
    {
        return command;
    }

    std::string Command_Interpreter::Get_SubCommand()
    {
        return subcommand;
    }

    std::vector<std::string> Command_Interpreter::Get_Flags()
    {
        return flags;
    }



}