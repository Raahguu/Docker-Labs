#include <iostream>
#include "cloudflare_cli.h"
#include "cloudflare_cli_tests.h"
#include <getopt.h>

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
        else if (command.Get_SubCommand() == "full-init") {
            return Commands::Test_Initialize(cf_auth);
        }
        else if (command.Get_SubCommand() == "grant") {
            return Commands::Test_Grant_Policy(cf_auth);
        }
    }
    else if (command.Get_Command() == "create") {
        if (command.Get_SubCommand() == "ingress") {
            return Commands::Create_Ingress(cf_auth, argc, argv);
        }
        else if (command.Get_SubCommand() == "dns") {
            return Commands::Create_DNS(cf_auth, argc, argv);
        }
        else if (command.Get_SubCommand() == "app" || command.Get_SubCommand() == "application") {
            return Commands::Create_Application(cf_auth, argc, argv);
        }
    }
    else if (command.Get_Command() == "remove") {
        if (command.Get_SubCommand() == "ingress") {
            return Commands::Remove_Ingress(cf_auth, argc, argv);
        }
        else if (command.Get_SubCommand() == "dns") {
            return Commands::Remove_DNS(cf_auth, argc, argv);
        }
        else if (command.Get_SubCommand() == "app" || command.Get_SubCommand() == "application") {
            return Commands::Remove_Application(cf_auth, argc, argv);
        }
    }
    else if (command.Get_Command() == "update") {
        return Commands::Update_Ingress(cf_auth, argc, argv);
    }
    else if (command.Get_Command() == "grant") {
        return Commands::Grant_Container(cf_auth, argc, argv);
    }
    else if (command.Get_Command() == "revoke") {
        return Commands::Revoke_Container(cf_auth, argc, argv);
    }
    return 1;
}


int Docker_Labs::Cloudflare::Commands::Get_Seats(API_Auth cf_auth)
{
    std::vector<Docker_Labs::User> users = Docker_Labs::Cloudflare::Fetch_Seats(cf_auth);
    for (Docker_Labs::User user : users) {
        std::cout << user.Get_Email() << std::endl;
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

int Docker_Labs::Cloudflare::Commands::Fetch_DNS_Records(API_Auth cf_auth) {
    json responce_body = Docker_Labs::Cloudflare::Fetch_DNS_Records(cf_auth);
    json records = responce_body["result"];
    std::string comment;
    json meta;
    for (const json record : records) {
        std::size_t pos = comment.find('>');
        std::string container = comment.substr(0, pos), owner = comment.substr(pos + 1);
        std::cout << "Name: " << record["name"] << std::endl;
        std::cout << "Container: " << container << std::endl;
        std::cout << "Owner: " << owner << std::endl;
        std::cout << "Proxy: " << record["content"] << std::endl << std::endl;
    }
    return 0;
}

Docker_Labs::Container Docker_Labs::Cloudflare::Commands::Spec_Container(int argc, char* argv[])
{
    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"container", required_argument, nullptr, 0},
        { nullptr, 0, nullptr, 0 }
    };

    std::string container_name = "";

    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "h", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "I belive in you, you can figure it out :)";
        case 0:
            if (std::string(long_flags[long_index].name) == "container")
                container_name = optarg;
        case '?':
        default:
            std::string temp = argv[optind - 1];
            while (!temp.empty() && temp[0] == '-') {
                temp.erase(0, 1);
            }
            std::cerr << "Unknown flag: " << temp << std::endl;
        }
    }
    
    Container cont = Docker_Labs::Docker::Get_Container(container_name);
    cont.Cache_Update();
    return cont;
}
std::tuple<Docker_Labs::Container, Docker_Labs::User> Docker_Labs::Cloudflare::Commands::Spec_Container_User(int argc, char* argv[])
{
    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"container", required_argument, nullptr, 0},
        {"user", required_argument, nullptr, 0},
        { nullptr, 0, nullptr, 0 }
    };

    std::string container_name = "";
    std::string user = "";

    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "h", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "I belive in you, you can figure it out :)";
        case 0:
            if (std::string(long_flags[long_index].name) == "container")
                container_name = optarg;
            else if (std::string(long_flags[long_index].name) == "user")
                user = optarg;
        case '?':
        default:
            std::string temp = argv[optind - 1];
            while (!temp.empty() && temp[0] == '-') {
                temp.erase(0, 1);
            }
            std::cerr << "Unknown flag: " << temp << std::endl;
        }
    }
    std::tuple<Container, User> cont_usr = std::tuple<Container, User>(Docker_Labs::Docker::Get_Container(container_name), User(user));
    std::get<Container>(cont_usr).Cache_Update();
    return cont_usr;
}

int Docker_Labs::Cloudflare::Commands::Create_Ingress(API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc, argv);
    return Cloudflare::Cloudflared(cf_auth).Create_Ingress(container);
}
int Docker_Labs::Cloudflare::Commands::Update_Ingress(API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Cloudflare::Cloudflared(cf_auth).Update_Ingress(container);
}
int Docker_Labs::Cloudflare::Commands::Remove_Ingress(API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Cloudflare::Cloudflared(cf_auth).Remove_Ingress(container);
}

int Docker_Labs::Cloudflare::Commands::Create_DNS(API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Cloudflare::Cloudflared(cf_auth).Create_DNS_Record(container);
}
int Docker_Labs::Cloudflare::Commands::Remove_DNS(API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Cloudflare::Cloudflared(cf_auth).Remove_DNS_Record(container);
}

int Docker_Labs::Cloudflare::Commands::Create_Application(API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Cloudflare::Cloudflared(cf_auth).Create_Application(container);
}
int Docker_Labs::Cloudflare::Commands::Remove_Application(API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Cloudflare::Cloudflared(cf_auth).Remove_Application(container);
}

int Docker_Labs::Cloudflare::Commands::Grant_Container(API_Auth cf_auth, int argc, char* argv[])
{
    std::tuple<Container, User> container_user = Spec_Container_User(argc, argv);
    return Cloudflare::Cloudflared(cf_auth).Grant_Container(std::get<Container>(container_user),std::get<User>(container_user));
}
int Docker_Labs::Cloudflare::Commands::Revoke_Container(API_Auth cf_auth, int argc, char* argv[])
{
    std::tuple<Container, User> container_user = Spec_Container_User(argc, argv);
    return Cloudflare::Cloudflared(cf_auth).Grant_Container(std::get<Container>(container_user), std::get<User>(container_user));
}

