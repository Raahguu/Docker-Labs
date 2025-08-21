#include <iostream>
#include "cloudflare_cli.h"
#include "base64.h"


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

    if (command.Get_Command() == "fetch") {
        if (command.Get_SubCommand() == "seats") {
            return Commands::Get_Seats(cf_auth);
        }
        else if (command.Get_SubCommand() == "ingress")
        {
            return Commands::Fetch_Ingress(cf_auth);
        }
        else if (command.Get_SubCommand() == "dns") {
            return Commands::Fetch_DNS_Records(cf_auth);
        }
    }
    else if (command.Get_Command() == "test") {

        if (command.Get_SubCommand() == "api") {
            return Commands::Test_API(cf_auth);
        }
        else if (command.Get_SubCommand() == "ingress") {
            return Commands::Test_Ingress(cf_auth);
        }
        else if (command.Get_SubCommand() == "dns") {
            return Commands::Test_DNS(cf_auth);
        }
        else if (command.Get_SubCommand() == "app" || command.Get_SubCommand() == "application") {
            return Commands::Test_Application(cf_auth);
        }
        else if (command.Get_SubCommand() == "policy") {
            return Commands::Test_Initial_Policy(cf_auth);
        }
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
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth);
    std::cout << "Creating bogus ingress rule." << std::endl;
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" }, "laith.striegher@cassa.au");
    cloudflared.Create_Ingress(container);
    std::cout << "Created." << std::endl << "Waiting 10 seconds before update..." << std::endl;
    sleep(10);
    container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "172.17.0.2", { "a", "b" }, "laith.striegher@cassa.au");
    cloudflared.Update_Ingress(container);
    std::cout << "Updated." << std::endl << "Waiting 10 seconds before removal..." << std::endl;
    sleep(10);
    cloudflared.Remove_Ingress(container);
    std::cout << "Removed." << std::endl;

    return 0;
}

int Docker_Labs::Cloudflare::Commands::Fetch_DNS_Records(API_Auth cf_auth) {
    json responce_body = Docker_Labs::Cloudflare::Fetch_DNS_Records(cf_auth);
    json records = responce_body["result"];
    std::string comment;
    json meta;
    for (const json record : records) {
        comment = BASE64::BASE64Decode(record["comment"]);
        std::size_t pos = comment.find('>');
        std::string container = comment.substr(0, pos), owner = comment.substr(pos + 1);
        std::cout << "Name: " << record["name"] << std::endl;
        std::cout << "Container: " << container << std::endl;
        std::cout << "Owner: " << owner << std::endl;
        std::cout << "Proxy: " << record["content"] << std::endl << std::endl;
    }
    return 0;
}

int Docker_Labs::Cloudflare::Commands::Test_DNS(API_Auth cf_auth)
{
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth);
    std::cout << "Creating bogus DNS record..." << std::endl;
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" }, "laith.striegher@cassa.au");
    cloudflared.Create_DNS_Record(container);
    std::cout << "Waiting 1 second for lookup..." << std::endl;
    sleep(1);
    Commands::Fetch_DNS_Records(cf_auth);
    std::cout << "Confirm the bogus record is in the list above." << std::endl << std::endl;
    std::cout << "Waiting 9 seconds before removal..." << std::endl;
    sleep(9);
    cloudflared.Remove_DNS_Record(container);
    std::cout << "Removed." << std::endl;

    return 0;
}
int Docker_Labs::Cloudflare::Commands::Test_Application(API_Auth cf_auth)
{
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth);
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" }, "laith.striegher@cassa.au");
    std::cout << "Creating bogus Access Application..." << std::endl;
    cloudflared.Create_Application(container);
    std::cout << "Waiting 10 second before removal..." << std::endl;
    sleep(10);
    cloudflared.Remove_Application(container);
    std::cout << "Removed." << std::endl;

    return 0;
}

int Docker_Labs::Cloudflare::Commands::Test_Initial_Policy(API_Auth cf_auth)
{
    Cloudflare::Cloudflared cloudflared = Cloudflare::Cloudflared(cf_auth);
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" }, "laith.striegher@cassa.au");
    cloudflared.Create_Ingress(container);
    cloudflared.Create_DNS_Record(container);
    cloudflared.Create_Application(container);
    cloudflared.Initialize_Policy(container);
    return 0;
}


