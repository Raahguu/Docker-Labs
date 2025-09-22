#include <vector>
#include "docker_labs/cli/cloudflare_cli.h"
#include "docker_labs/cli/cloudflare_cli_tests.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"
#include "boost/program_options.hpp"
#ifdef _WIN32
#include <io.h>
#define isPiped() !_isatty(_fileno(stdin))
#else
#include <unistd.h>
#define isPiped() !isatty(fileno(stdin))
#endif

using namespace Docker_Labs;
namespace po = boost::program_options;

Labs_Core::Cloudflare::API_Auth Labs_CLI::Cloudflare::Get_Auth() {
    std::string conn_str;
    std::cin >> conn_str;
    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_Core::Cloudflare::API_Auth::From_Connection_String(conn_str);
    return cf_auth;
}

Labs_Core::Cloudflare::API_Auth Labs_CLI::Cloudflare::Get_Auth_Legacy()
{
    if (!isPiped()) {
        std::cout << "Error. Cloudflare keys must be piped." << std::endl;
        exit(1);
    }
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

    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_Core::Cloudflare::API_Auth(
        ACC,
        ZONE,
        TUNN,
        TKN,
        DOMN
    );

    return cf_auth;
}

int Labs_CLI::Cloudflare::Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]) {
    std::string command_str = command.Get_Command();

    if (command_str == "create_conn_str") {
        Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth_Legacy();
        std::cout << cf_auth.Generate_Connection_String() << std::endl;
        exit(0);
    }

    Labs_Core::Cloudflare::API_Auth cf_auth = Labs_CLI::Cloudflare::Get_Auth();

    // Sub-command mappings for fetch operations
    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>> fetch_commands = {
        //                                     ^ A ^---------------------------- B
        {"seats",       Fetch_Seats},
        {"ingress",     Fetch_Ingress},
        {"dns",         Fetch_DNS_Records}
        //^ C           ^-------------- D

        // A: the return type of the sub-command functions.
        // B: the parameter type of the sub-command functions.
        // C: the command-line sub-command.
        // D: the program function to execute with the parameters in 'B'.
    };

    // Sub-command mappings for test operations
    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>> test_commands = {
        {"api",         Test_API},
        {"ingress",     Test_Ingress},
        {"dns",         Test_DNS},
        {"app",         Test_Application},
        {"application", Test_Application},
        {"full-init",   Test_Initialize},
        {"grant",       Test_Grant_Policy}
    };

    // Sub-command mappings for create operations
    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>> create_commands = {
        {"ingress",     Create_Ingress},
        {"dns",         Create_DNS},
        {"app",         Create_Application},
        {"application", Create_Application}
    };

    // Sub-command mappings for remove operations
    static std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>> remove_commands = {
        {"ingress",     Remove_Ingress},
        {"dns",         Remove_DNS},
        {"app",         Remove_Application},
        {"application", Remove_Application}
    };

    // Cloudflare partition commands
    if (command_str == "fetch") {
        // Map to fetch sub-commands
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>>::iterator
            it = fetch_commands.find(command.Get_SubCommand());

        if (it != fetch_commands.end()) {
            return it->second(cf_auth);
        }
    }
    else if (command_str == "test") {
        std::cout << "test" << std::endl;

        // Map to test sub-commands
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth)>>::iterator
            it = test_commands.find(command.Get_SubCommand());

        if (it != test_commands.end()) {
            return it->second(cf_auth);
        }
    }
    else if (command_str == "create") {
        // Map to create sub-commands
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>>::iterator
            it = create_commands.find(command.Get_SubCommand());

        if (it != create_commands.end()) {
            return it->second(cf_auth, argc, argv);
        }
    }
    else if (command_str == "remove") {
        // Map to remove sub-commands
        std::map<std::string, std::function<int(Labs_Core::Cloudflare::API_Auth, int, char**)>>::iterator
            it = remove_commands.find(command.Get_SubCommand());

        if (it != remove_commands.end()) {
            return it->second(cf_auth, argc, argv);
        }
    }
    else if (command_str == "update") {
        // Map to update
        return Update_Ingress(cf_auth, argc, argv);
    }
    else if (command_str == "grant") {
        // Map to grant
        return Grant_Container(cf_auth, argc, argv);
    }
    else if (command_str == "revoke") {
        // Map to revoke
        return Revoke_Container(cf_auth, argc, argv);
    }
    else if (command_str == "deactivate") {
        // Map to deactivate user
        return Deactivate_Seats(cf_auth, argc, argv);
    }

    return Help_Message(argc, argv);
}


int Labs_CLI::Cloudflare::Fetch_Seats(Labs_Core::Cloudflare::API_Auth cf_auth)
{
    std::vector<Labs_Core::User_Seat> users = Labs_Core::Cloudflare(cf_auth).Fetch_Seats();
    for (Labs_Core::User_Seat user : users) {
        std::cout << "Seat UID: " << user.Get_SeatUID() << std::endl;
        std::cout << "Name: " << user.Get_Name() << std::endl;
        std::cout << "Email: " << user.Get_Email() << std::endl;
        std::cout << "Created: " << user.Get_CreatedAt() << std::endl;
        std::cout << "Last Accessed: " << user.Get_LastSuccessfulLogin() << std::endl;
        std::cout << "Seats: " << (user.Get_AccessSeat() ? "Access" : "")
            << (user.Get_AccessSeat() && user.Get_GatewaySeat() ? ", " : "")
            << (user.Get_GatewaySeat() ? "Gateway" : "")
            << (user.Get_AccessSeat() || user.Get_GatewaySeat() ? "" : "None") << std::endl << std::endl;
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

    std::string container_name;
    // Define the options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Shows this popup")
        ("container,c", po::value<std::string>(&container_name)->required(), "Specifies the name of the container");

    // Parse the command line arguments
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm); // Throws if required options are missing
    }
    catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        exit(1);
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    }

    Labs_Core::Container cont = docker.Get_Container(container_name);
    cont.Cache_Update();
    return cont;
}
std::vector<Labs_Core::User> Labs_CLI::Cloudflare::Spec_User(int argc, char* argv[])
{
    std::vector<std::string> emails;
    std::vector<Labs_Core::User> users;
    // Define the options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Shows this popup")
        ("user,u", po::value<std::vector<std::string>>(&emails)->multitoken(), "Specifies the user by email");

    // Parse the command line arguments
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm); // Throws if required options are missing
    }
    catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        exit(1);
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    }
    else if (emails.size() > 0) {
        for (std::string email : emails) {
            users.push_back(Labs_Core::User(email));
        }
        return users;
    }
    std::cout << desc << std::endl;
    exit(1);
}
std::vector<Labs_Core::User_Seat> Labs_CLI::Cloudflare::Spec_Seat(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]) {
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);
    std::vector<std::string> emails;
    std::vector<Labs_Core::User_Seat> seated_users;
    // Define the options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Shows this popup")
        ("user,u", po::value<std::vector<std::string>>(&emails)->multitoken(), "Specifies the user by email")
        ("all,a", "Executes on all seated users.");

        // Parse the command line arguments
        po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm); // Throws if required options are missing
    }
    catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        exit(1);
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(0);
    }
    else if (vm.count("all")) {
        return Labs_Core::Cloudflare(cf_auth).Fetch_Seats();
    }
    else if (emails.size() > 0) {
        for (std::string email : emails) {
            seated_users.push_back(cloudflare.Fetch_Seat(Labs_Core::User(email)));
        }
        return seated_users;
    }

    std::cout << desc << std::endl;
    exit(0);
}
std::tuple<Labs_Core::Container, Labs_Core::User> Labs_CLI::Cloudflare::Spec_Container_User(int argc, char* argv[])
{
    Labs_Core::Docker docker = Labs_Core::Docker();
    std::string container_name = "";
    std::string user = "";

    // Define the options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Shows this popup")
        ("container,c", po::value<std::string>(&container_name)->required(), "Specifies the container name")
        ("user,u", po::value<std::string>(&user)->required(), "Specify the email of the user");

    // Parse the command line arguments
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm); // Throws if required options are missing
    }
    catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        exit(1);
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(1);
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
int Labs_CLI::Cloudflare::Deactivate_Seats(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]) {
    std::vector<Labs_Core::User_Seat> seated_users = Spec_Seat(cf_auth, argc, argv);
    
    for (Labs_Core::User_Seat seat: seated_users) {
        Labs_Core::Cloudflare(cf_auth).Deactivate_Seat(seat);
    }

    return 0;
}

int Labs_CLI::Cloudflare::Help_Message(int argc, char* argv[]) {
    std::cout << "INSERT HELP MESSAGE HERE" << std::endl;
    return 0;
}