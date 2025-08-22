#include "container_init.h"
#include "labs_container.h"
#include "labs_user.h"
#include "cloudflare_hook.h"
#include <string>
#include <iostream>
#include <getopt.h>
#include <random>


Docker_Labs::Cloudflare::API_Auth Docker_Labs::Get_Auth()
{
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

    return Docker_Labs::Cloudflare::API_Auth(
        ACC,
        ZONE,
        TUNN,
        TKN,
        DOMN
    );
}

int Docker_Labs::Init_Handler(int argc, char* argv[])
{
    std::random_device rd;
    std::mt19937 gen(rd());


    std::uniform_int_distribution<> dist(0, 35);

    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"user", required_argument, nullptr, 0},
        {"image", required_argument, nullptr, 0},
        {"name", required_argument, nullptr, 0},
        { nullptr, 0, nullptr, 0 }
    };

    static const std::string chars =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";

    std::string email = "";
    std::string image = "";
    std::string container_name = "";

    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "h", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "I belive in you, you can figure it out :)";
        case 0:
            if (std::string(long_flags[long_index].name) == "user")
                email = optarg;
            else if (std::string(long_flags[long_index].name) == "image")
                image = optarg;
        case '?':
        default:
            std::string temp = argv[optind - 1];
            while (!temp.empty() && temp[0] == '-') {
                temp.erase(0, 1);
            }
            //std::cerr << "Unknown flag: " << temp << std::endl;
        }
    }

    if (container_name.size() == 0) {
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


    Docker_Labs::Container container = Container(container_name, image);
    Cloudflare::API_Auth cf_auth = Get_Auth();
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth);
    cloudflared.Init_Access(container, User(email));
    return 0;
}
