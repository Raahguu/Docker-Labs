#include <iostream>
#include "cloudflare_cli.h"
#include "cloudflare_cli_tests.h"
#include <getopt.h>

int Docker_Labs::Labs_CLI::Cloudflare::Command_Handler(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
    Docker_Labs::Cloudflare::API_Auth cf_auth = Docker_Labs::Cloudflare::API_Auth::Get_Auth();

    static std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth)>> fetch_commands = {
        {"seats",       Get_Seats},
        {"ingress",     Fetch_Ingress},
        {"dns",         Fetch_DNS_Records}
    };

    static std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth)>> test_commands = {
        {"api",         Test_API},
        {"ingress",     Test_Ingress},
        {"dns",         Test_DNS},
        {"app",         Test_Application},
        {"application", Test_Application},
        {"full-init",   Test_Initialize},
        {"grant",       Test_Grant_Policy}
    };

    static std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth, int, char**)>> create_commands = {
        {"ingress",     Create_Ingress},
        {"dns",         Create_DNS},
        {"app",         Create_Application},
        {"application", Create_Application}
    };

    static std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth, int, char**)>> remove_commands = {
        {"ingress",     Remove_Ingress},
        {"dns",         Remove_DNS},
        {"app",         Remove_Application},
        {"application", Remove_Application}
    };

    if (command.Get_Command() == "fetch") {
        std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth)>>::iterator
            it = fetch_commands.find(command.Get_SubCommand());

        if (it != fetch_commands.end()) {
            return it->second(cf_auth);
        }
    }
    else if (command.Get_Command() == "test") {
        std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth)>>::iterator
            it = test_commands.find(command.Get_SubCommand());

        if (it != test_commands.end()) {
            return it->second(cf_auth);
        }
    }
    else if (command.Get_Command() == "create") {
        std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth, int, char**)>>::iterator
            it = create_commands.find(command.Get_SubCommand());

        if (it != create_commands.end()) {
            return it->second(cf_auth, argc, argv);
        }
    }
    else if (command.Get_Command() == "remove") {
        std::map<std::string, std::function<int(Docker_Labs::Cloudflare::API_Auth, int, char**)>>::iterator
            it = remove_commands.find(command.Get_SubCommand());

        if (it != remove_commands.end()) {
            return it->second(cf_auth, argc, argv);
        }
    }
    else if (command.Get_Command() == "update") {
        return Update_Ingress(cf_auth, argc, argv);
    }
    else if (command.Get_Command() == "grant") {
        return Grant_Container(cf_auth, argc, argv);
    }
    else if (command.Get_Command() == "revoke") {
        return Revoke_Container(cf_auth, argc, argv);
    }
    return 1;
}


int Docker_Labs::Labs_CLI::Cloudflare::Get_Seats(Docker_Labs::Cloudflare::API_Auth cf_auth)
{
    std::vector<Docker_Labs::User> users = Docker_Labs::Cloudflare::Fetch_Seats(cf_auth);
    for (Docker_Labs::User user : users) {
        std::cout << user.Get_Email() << std::endl;
    }
    return 0;
}

int Docker_Labs::Labs_CLI::Cloudflare::Fetch_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth)
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

int Docker_Labs::Labs_CLI::Cloudflare::Fetch_DNS_Records(Docker_Labs::Cloudflare::API_Auth cf_auth) {
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

Docker_Labs::Container Docker_Labs::Labs_CLI::Cloudflare::Spec_Container(int argc, char* argv[])
{
    Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"container", required_argument, nullptr, 'c'},
        { nullptr, 0, nullptr, 0 }
    };

    std::string container_name = "";

    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "hc:", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "-h, --help: shows this popup" << std::endl;
            std::cout << "-c, --container: [Required] specifies the name of the container" << std::endl;
            throw 0;
        case 'c':
            container_name = optarg;
            continue;
        case '?':
        default:
            std::string temp = argv[optind - 1];
            while (!temp.empty() && temp[0] == '-') {
                temp.erase(0, 1);
            }
            std::cerr << "Unknown flag: " << temp << std::endl;
            throw 0;
        }
    }
    
    if (container_name == "") {
    	std::cerr << "You must specify the container name" << std::endl;
    	throw 0;
    }
    
    Container cont = docker.Get_Container(container_name);
    cont.Cache_Update();
    return cont;
}
std::tuple<Docker_Labs::Container, Docker_Labs::User> Docker_Labs::Labs_CLI::Cloudflare::Spec_Container_User(int argc, char* argv[])
{
    Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
    static struct option long_flags[] = {
        {"help", no_argument, nullptr, 'h'},
        {"container", required_argument, nullptr, 'c'},
        {"user", required_argument, nullptr, 'u'},
        { nullptr, 0, nullptr, 0 }
    };

    std::string container_name = "";
    std::string user = "";

    int opt;
    int long_index;
    while ((opt = getopt_long(argc, argv, "hc:u:", long_flags, &long_index)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "-h, --help: Shows this popup" << std::endl;
            std::cout << "-c, --container: [Required] Specifies the container name" << std::endl;
            std::cout << "-u, --user: [Required] Specify the email of the user" << std::endl;
            throw 0;
        case 'c':
            container_name = optarg;
            continue;
        case 'u':
            user = optarg;
            continue;
        case '?':
        default:
            std::string temp = argv[optind - 1];
            while (!temp.empty() && temp[0] == '-') {
                temp.erase(0, 1);
            }
            std::cerr << "Unknown flag: " << temp << std::endl;
            throw 0;
        }
    }
    
    if (container_name == "") {
    	std::cerr << "must specify a container name" << std::endl;
    	throw 0;
    }
    if (user == "") {
    	std::cerr << "must specify a user" << std::endl;
    	throw 0;
    }
    
    std::tuple<Container, User> cont_usr = std::tuple<Container, User>(docker.Get_Container(container_name), User(user));
    std::get<Container>(cont_usr).Cache_Update();
    return cont_usr;
}

int Docker_Labs::Labs_CLI::Cloudflare::Create_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc, argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Create_Ingress(container);
}
int Docker_Labs::Labs_CLI::Cloudflare::Update_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Update_Ingress(container);
}
int Docker_Labs::Labs_CLI::Cloudflare::Remove_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Remove_Ingress(container);
}

int Docker_Labs::Labs_CLI::Cloudflare::Create_DNS(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Create_DNS_Record(container);
}
int Docker_Labs::Labs_CLI::Cloudflare::Remove_DNS(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Remove_DNS_Record(container);
}

int Docker_Labs::Labs_CLI::Cloudflare::Create_Application(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc,argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Create_Application(container);
}
int Docker_Labs::Labs_CLI::Cloudflare::Remove_Application(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Container container = Spec_Container(argc, argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Remove_Application(container);
}

int Docker_Labs::Labs_CLI::Cloudflare::Grant_Container(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    std::tuple<Container, User> container_user = Spec_Container_User(argc, argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Grant_Container(std::get<Container>(container_user),std::get<User>(container_user));
}
int Docker_Labs::Labs_CLI::Cloudflare::Revoke_Container(Docker_Labs::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    std::tuple<Container, User> container_user = Spec_Container_User(argc, argv);
    return Docker_Labs::Cloudflare::Cloudflared(cf_auth).Revoke_Container(std::get<Container>(container_user), std::get<User>(container_user));
}

