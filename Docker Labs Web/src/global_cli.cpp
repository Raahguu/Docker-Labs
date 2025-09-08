#include <random>
#include "global_cli.h"
#include "web_server.h"
#include <getopt.h>


using namespace Docker_Labs;

int Docker_Labs::Labs_Web::Global_Handler(Docker_Labs::Labs_Web::Command_Interpreter command, int argc, char* argv[]) {
    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, 0 }
    };


    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "hu:i:n:k", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            return 0;
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
    
    Labs_Web::Socket socket = Labs_Web::Socket();
    socket.Listen();

    return 1;
}
