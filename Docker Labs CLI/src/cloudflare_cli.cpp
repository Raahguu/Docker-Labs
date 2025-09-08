#include "cloudflare_cli.h"
#include "cloudflare_cli_tests.h"
#include "cloudflare_hook.h"
#include "docker_hook.h"

using namespace Docker_Labs;

int Labs_CLI::Cloudflare::Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_Core::Cloudflare::API_Auth::Get_Auth();

    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>> fetch_commands = {
        {"seats",       Get_Seats},
        {"ingress",     Fetch_Ingress},
        {"dns",         Fetch_DNS_Records}
    };

    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>> test_commands = {
        {"api",         Test_API},
        {"ingress",     Test_Ingress},
        {"dns",         Test_DNS},
        {"app",         Test_Application},
        {"application", Test_Application},
        {"full-init",   Test_Initialize},
        {"grant",       Test_Grant_Policy}
    };

    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>> create_commands = {
        {"ingress",     Create_Ingress},
        {"dns",         Create_DNS},
        {"app",         Create_Application},
        {"application", Create_Application}
    };

    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>> remove_commands = {
        {"ingress",     Remove_Ingress},
        {"dns",         Remove_DNS},
        {"app",         Remove_Application},
        {"application", Remove_Application}
    };

    if (command.Get_Command() == "fetch") {
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>>::iterator
            it = fetch_commands.find(command.Get_SubCommand());

        if (it != fetch_commands.end()) {
            return it->second(cf_auth);
        }
    }
    else if (command.Get_Command() == "test") {
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>>::iterator
            it = test_commands.find(command.Get_SubCommand());

        if (it != test_commands.end()) {
            return it->second(cf_auth);
        }
    }
    else if (command.Get_Command() == "create") {
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>>::iterator
            it = create_commands.find(command.Get_SubCommand());

        if (it != create_commands.end()) {
            return it->second(cf_auth, argc, argv);
        }
    }
    else if (command.Get_Command() == "remove") {
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>>::iterator
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


int Labs_CLI::Cloudflare::Get_Seats(Labs_Core::Cloudflare::API_Auth cf_auth)
{
    std::vector<Labs_Core::User> users = Labs_Core::Cloudflare::Fetch_Seats(cf_auth);
    for (Labs_Core::User user : users) {
        std::cout << user.Get_Email() << std::endl;
    }
    return 0;
}

int Labs_CLI::Cloudflare::Fetch_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth)
{
    json responce_body = Labs_Core::Cloudflare::Fetch_Ingress(cf_auth);
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

int Labs_CLI::Cloudflare::Fetch_DNS_Records(Labs_Core::Cloudflare::API_Auth cf_auth) {
    json responce_body = Labs_Core::Cloudflare::Fetch_DNS_Records(cf_auth);
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

Labs_Core::Container Labs_CLI::Cloudflare::Spec_Container(int argc, char* argv[])
{
    Labs_Core::Docker docker = Labs_Core::Docker();
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
    
    Labs_Core::Container cont = docker.Get_Container(container_name);
    cont.Cache_Update();
    return cont;
}
std::tuple<Labs_Core::Container, Labs_Core::User> Labs_CLI::Cloudflare::Spec_Container_User(int argc, char* argv[])
{
    Labs_Core::Docker docker = Labs_Core::Docker();
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
    
    std::tuple<Labs_Core::Container, Labs_Core::User> cont_usr = std::tuple<Labs_Core::Container, Labs_Core::User>(docker.Get_Container(container_name), Labs_Core::User(user));
    std::get<Labs_Core::Container>(cont_usr).Cache_Update();
    return cont_usr;
}

int Labs_CLI::Cloudflare::Create_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Labs_Core::Container container = Spec_Container(argc, argv);
    return Labs_Core::Cloudflare(cf_auth).Create_Ingress(container);
}
int Labs_CLI::Cloudflare::Update_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Labs_Core::Container container = Spec_Container(argc,argv);
    return Labs_Core::Cloudflare(cf_auth).Update_Ingress(container);
}
int Labs_CLI::Cloudflare::Remove_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Labs_Core::Container container = Spec_Container(argc,argv);
    return Labs_Core::Cloudflare(cf_auth).Remove_Ingress(container);
}

int Labs_CLI::Cloudflare::Create_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Labs_Core::Container container = Spec_Container(argc,argv);
    return Labs_Core::Cloudflare(cf_auth).Create_DNS_Record(container);
}
int Labs_CLI::Cloudflare::Remove_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Labs_Core::Container container = Spec_Container(argc,argv);
    return Labs_Core::Cloudflare(cf_auth).Remove_DNS_Record(container);
}

int Labs_CLI::Cloudflare::Create_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Labs_Core::Container container = Spec_Container(argc,argv);
    return Labs_Core::Cloudflare(cf_auth).Create_Application(container);
}
int Labs_CLI::Cloudflare::Remove_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    Labs_Core::Container container = Spec_Container(argc, argv);
    return Labs_Core::Cloudflare(cf_auth).Remove_Application(container);
}

int Labs_CLI::Cloudflare::Grant_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    std::tuple<Labs_Core::Container, Labs_Core::User> container_user = Spec_Container_User(argc, argv);
    return Labs_Core::Cloudflare(cf_auth).Grant_Container(std::get<Labs_Core::Container>(container_user), std::get<Labs_Core::User>(container_user));
}
int Labs_CLI::Cloudflare::Revoke_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[])
{
    std::tuple<Labs_Core::Container, Labs_Core::User> container_user = Spec_Container_User(argc, argv);
    return Labs_Core::Cloudflare(cf_auth).Revoke_Container(std::get<Labs_Core::Container>(container_user), std::get<Labs_Core::User>(container_user));
}

