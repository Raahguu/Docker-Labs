#include <tuple>
#include <iostream>
#include <filesystem>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"
#include "docker_labs/core/curl_wrapper.h"
#include "docker_labs/core/labs_user.h"

using namespace Docker_Labs;

//
// ==== Seat Deactivation Generators ====
//

// Generate seat deactivation JSON for a single user seat
std::string Labs_Core::Cloudflare::Generate_Seat_Deactivation(Labs_Core::User_Seat user)
{
    json message_body = "[{\"access_seat\":false,\"gateway_seat\":false}]"_json;
    message_body[0]["seat_uid"] = user.Get_SeatUID();
    return message_body.dump();
}

// Generate bulk seat deactivation JSON for multiple user seats
std::string Labs_Core::Cloudflare::Generate_Bulk_Seat_Deactivation(std::vector<Labs_Core::User_Seat> users)
{
    json message_body = "[]"_json;
    for (Labs_Core::User_Seat& user : users) {
        json user_object = "{\"access_seat\":false,\"gateway_seat\":false}"_json;
        user_object["seat_uid"] = user.Get_SeatUID();
        message_body.push_back(user_object);
    }
    return message_body.dump();
}

//
// ==== Container Access Management ====
//

// Grant user access to container by updating Cloudflare application policy
int Labs_Core::Cloudflare::Grant_Container(Container container, User user)
{
    json application = Fetch_Application(container);
    std::string application_id = application["result"][0]["id"];
    json application_policy = Fetch_Application_Policy(container, application_id);

    if (application_policy["result"].empty()) {
        return Initialize_Policy(container, user, application_id);
    }
    else {
        std::string application_policy_id = application_policy["result"][0]["id"];
        std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies/" + application_policy_id;
        std::vector<std::string> headers = {
            "Content-Type: application/json",
            "Authorization: Bearer " + auth.TKN
        };
        std::string data = Generate_Grant_Policy_Config(container, user, application_policy);
        std::string response = std::get<std::string>(curl.Put(url, data, headers));
    }

    if (must_cout) {
        std::cout << "Granted access to '" << user.Get_Email() << "' for container '" << container.Get_Name_Cache() << "'\n";
        std::cout << "Application ID: '" << application_id << "'\n";
    }
    return 0;
}

// Revoke user access from container application policy
int Labs_Core::Cloudflare::Revoke_Container(Container container, User user)
{
    json application = Fetch_Application(container);
    std::string application_id = application["result"][0]["id"];
    json application_policy = Fetch_Application_Policy(container, application_id);
    std::string application_policy_id = application_policy["result"][0]["id"];

    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies/" + application_policy_id;
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "Authorization: Bearer " + auth.TKN
    };
    std::string data = Generate_Revoke_Policy_Config(container, user, application_policy);
    std::string response = std::get<std::string>(curl.Put(url, data, headers));

    auto return_info = Get_Return_Info(json::parse(response));
    if (std::get<0>(return_info[0]) && must_cout) {
        std::cout << "Revoked access to '" << user.Get_Email() << "' for container '" << container.Get_Name_Cache() << "'\n";
        std::cout << "Application ID: '" << application_id << "'\n";
    }
    else {
        std::cout << "Failed to revoke access to '" << user.Get_Email() << "' for container '" << container.Get_Name_Cache() << "'\n";
        std::cout << "Application ID: '" << application_id << "'\n";
        for (const auto& err : return_info) {
            std::cout << "Code: " << std::get<int>(err) << "; Message: " << std::get<std::string>(err) << ";\n";
        }
    }
    return std::get<0>(return_info[0]);
}

//
// ==== Seat Activation and Deactivation ====
//

// Deactivate a single user seat
int Labs_Core::Cloudflare::Deactivate_Seat(User_Seat seated_user)
{
    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/seats";
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "Authorization: Bearer " + auth.TKN
    };

    std::string data = Generate_Seat_Deactivation(seated_user);
    std::string response = std::get<std::string>(curl.Patch(url, data, headers));
    return 0;
}

// Activate a container for a user (start container, create ingress, DNS, application, policy)
int Labs_Core::Cloudflare::Activate_Container(Container container, User user)
{
    Labs_Core::Docker docker;
    bool err_count = 0; // Using bool as integer accumulator, clever :)

    docker.Start(container);
    container.Cache_Update();
    std::cout << "Fetched container information.\nDeploying to cloudflare...\n";

    err_count += Create_Ingress(container);
    std::cout << " - Created ingress rule.\n";

    err_count += Create_DNS_Record(container);
    std::cout << " - Created DNS record.\n";

    std::string app_name = "SSH Application for '" + user.Get_Email() + "' (" +
        container.Get_Name_Cache().substr(container.Get_Name_Cache().length() - 3) + ")";
    err_count += Create_Application(container, app_name);
    std::cout << " - Created Access application.\n";

    err_count += Initialize_Policy(container, user);
    std::cout << " - Assigned default Access policy.\n";
    std::cout << " - Granted access to '" << user.Get_Email() << "'\n";

    std::cout << "Finished. Access container from https://" + container.Get_Name_Cache() + "-" + auth.DOMN << std::endl;
    return err_count;
}

// Deactivate container, optionally removing it
int Docker_Labs::Labs_Core::Cloudflare::Deactivate_Container(Container container)
{
    return Deactivate_Container(container, true);
}

int Docker_Labs::Labs_Core::Cloudflare::Deactivate_Container(Container container, bool keep_container)
{
    container.Cache_Update();
    Remove_Application(container);
    Remove_DNS_Record(container);
    Remove_Ingress(container);

    if (!keep_container) {
        container.Remove();
    }
    else {
        container.Stop();
    }

    return 0;
}

//
// ==== Fetching Seats and Users ====
//

// Fetch all seats (users with access)
std::vector<Labs_Core::User_Seat> Labs_Core::Cloudflare::Fetch_Seats()
{
    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/users";
    std::vector<std::string> headers = {
        "Authorization: Bearer " + auth.TKN
    };
    std::string response = std::get<std::string>(curl.Get(url, headers));
    json seats = json::parse(response)["result"];

    std::vector<Labs_Core::User_Seat> users;
    for (const json& user : seats) {
        users.push_back(User_Seat(user));
    }
    return users;
}

// Fetch users with seats (wrapper over Fetch_Seats)
std::vector<Labs_Core::User> Labs_Core::Cloudflare::Fetch_Seated()
{
    std::vector<Labs_Core::User> users;
    for (const Labs_Core::User_Seat& seated_user : Fetch_Seats()) {
        users.push_back(seated_user);
    }
    return users;
}

// Fetch seat for a specific user by email
Labs_Core::User_Seat Labs_Core::Cloudflare::Fetch_Seat(Labs_Core::User user)
{
    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/users?email=" + user.Get_Email();
    std::vector<std::string> headers = {
        "Authorization: Bearer " + auth.TKN
    };
    std::string response = std::get<std::string>(curl.Get(url, headers));
    json users = json::parse(response)["result"];

    if (users.empty()) {
        throw std::runtime_error("User does not have a seat.");
    }
    return User_Seat(users[0]);
}
