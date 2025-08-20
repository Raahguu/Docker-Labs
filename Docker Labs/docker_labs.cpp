// Docker Labs.cpp : Defines the entry point for the application.
//

#include "docker_labs.h"
#include "cloudflare_hook.h"
#include <iostream>
#include <string_view>


int main(int argc, char* argv[])
{   
    using namespace std::literals;
    using namespace Docker_Labs;

    Docker_Labs::Command_Interpreter command = Docker_Labs::Command_Interpreter(argc, argv);
    
    if (command.Get_Partition() == "cloudflare"sv) {

        std::string ACC;
        std::string ZONE;
        std::string TUNN;
        std::string TKN;

        std::cin >> ACC;
        std::cin >> ZONE;
        std::cin >> TUNN;
        std::cin >> TKN;

        Docker_Labs::Cloudflare::API_Auth cf_auth = Docker_Labs::Cloudflare::API_Auth(
            ACC,
            ZONE,
            TUNN,
            TKN
        );

        if (command.Get_Command() == "test-api"sv)
        {
            std::cout << "Sending Reqest..." << std::endl;
            std::cout << Docker_Labs::Cloudflare::Test_API(cf_auth) << std::endl;
        }
    }
   
    return 0;
}


namespace Docker_Labs {


    Command_Interpreter::Command_Interpreter(int argc, char* argv[]) {

        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <command> [subcommand] [sub_subcommand] [flags...]\n";
            
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

            // Collect flags
            for (; index < argc; ++index) {
                std::string arg = argv[index];
                if (arg.rfind("-", 0) == 0) { // starts with '-' or '--'
                    flags.push_back(arg);
                }
                else {
                    std::cerr << "Unexpected argument: " << arg << "\n";
                }
            }

            // Output parsed values
            //std::cout << "Command: " << partition << "\n";
            //if (!command.empty()) std::cout << "Subcommand: " << command << "\n";
            //if (!subcommand.empty()) std::cout << "Sub-subcommand: " << subcommand << "\n";

            //std::cout << "Flags:\n";
            //for (const auto& f : flags) {
            //    std::cout << "  " << f << "\n";
            //}
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