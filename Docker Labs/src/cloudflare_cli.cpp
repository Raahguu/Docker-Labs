#include <iostream>
#include <string_view>
#include "cloudflare_cli.h"

using namespace std::literals;

int Docker_Labs::Cloudflare::Commands::Command_Handler(Command_Interpreter command, int argc, char* argv[]) {
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
        return Commands::Test_API(cf_auth);
    }
    else if (command.Get_Command() == "get-seats"sv) {
        return Commands::Get_Seats(cf_auth);
    }
    else if (command.Get_Command() == "fetch-ingress"sv)
    {
        return Commands::Fetch_Ingress(cf_auth);
    }
    else if (command.Get_Command() == "test_ingress") {
        return Commands::Test_Ingress(cf_auth);
    }
    return 1;
}

int Docker_Labs::Cloudflare::Commands::Test_API(API_Auth cf_auth) {
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

int Docker_Labs::Cloudflare::Commands::Get_Seats(API_Auth cf_auth)
{
    std::vector<Docker_Labs::User> users = Docker_Labs::Cloudflare::Get_Seats(cf_auth);
    for (Docker_Labs::User user : users) {
        std::cout << user.Get_Email() << " " << user.Get_CF_UID() << std::endl;
    }
    return 0;
}

int Docker_Labs::Cloudflare::Commands::Fetch_Ingress(API_Auth cf_auth)
{
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
    return 0;
}

int Docker_Labs::Cloudflare::Commands::Test_Ingress(API_Auth cf_auth)
{
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_e3t", "image", "127.0.0.1", { "a", "b" }, "laith.striegher@cassa.au");
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth);
    cloudflared.Create_Ingress(container);
    return 0;
}
