/*
 * cloudflare_ingress_dns.cpp
 *
 * This file contains implementations for managing Cloudflare Tunnel ingress configurations
 * and DNS records via the Cloudflare API, using the Docker_Labs framework.
 *
 * Features:
 *  - Fetch, create, update, and remove ingress rules for Cloudflare tunnels
 *  - Manage DNS records (CNAME) for containers within the Cloudflare zone
 *
 * Dependencies:
 *  - Boost Base64 iterators
 *  - Docker_Labs core modules (cloudflare_hook, docker_hook, curl_wrapper, labs_user)
 *  - nlohmann/json for JSON parsing (assumed included transitively)
 */

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
// Ingress Configuration Management
//

// Fetch the full ingress configuration JSON for the Cloudflare tunnel
json Labs_Core::Cloudflare::Fetch_Ingress() {
    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/" + auth.TUNN + "/configurations";
    std::vector<std::string> headers = {
        "Authorization: Bearer " + auth.TKN
    };
    std::string response = std::get<std::string>(curl.Get(url, headers));
    return json::parse(response);
}

// Fetch only the ingress array within the tunnel's configuration
json Labs_Core::Cloudflare::Fetch_Ingress_Config() {
    return Fetch_Ingress()["result"]["config"]["ingress"];
}

// Create a new ingress rule for a container
int Labs_Core::Cloudflare::Create_Ingress(Container container) {
    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/" + auth.TUNN + "/configurations";
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "Authorization: Bearer " + auth.TKN
    };

    std::string data = Generate_Add_Ingress_Config(container);
    std::string response = std::get<std::string>(curl.Put(url, data, headers));

    if (must_cout) {
        std::cout << "Created rule for '" << container.Get_Name_Cache() << "'" << std::endl;
    }
    return 0;
}

// Remove an ingress rule for a container
int Labs_Core::Cloudflare::Remove_Ingress(Container container) {
    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/" + auth.TUNN + "/configurations";
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "Authorization: Bearer " + auth.TKN
    };

    std::string data = Generate_Remove_Ingress_Config(container);
    std::string response = std::get<std::string>(curl.Put(url, data, headers));

    if (must_cout) {
        std::cout << "Removed rule for '" << container.Get_Name_Cache() << "'" << std::endl;
    }
    return 0;
}

// Update an existing ingress rule for a container (remove + add)
int Labs_Core::Cloudflare::Update_Ingress(Container container) {
    std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/" + auth.TUNN + "/configurations";
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "Authorization: Bearer " + auth.TKN
    };

    std::string data = Generate_Update_Ingress_Config(container);
    std::string response = std::get<std::string>(curl.Put(url, data, headers));

    if (must_cout) {
        std::cout << "Updated rule for '" << container.Get_Name_Cache() << "'" << std::endl;
    }
    return 0;
}

// Generate JSON config to add an ingress rule for a container
std::string Labs_Core::Cloudflare::Generate_Add_Ingress_Config(Container container) {
    json current_ingress = Fetch_Ingress_Config();
    return Generate_Add_Ingress_Config(container, current_ingress);
}

// Generate JSON config to add an ingress rule given current ingress array
std::string Labs_Core::Cloudflare::Generate_Add_Ingress_Config(Container container, json current_ingress) {
    // Example for path-based ingress (commented out)
    // std::string path = "container/" + container.Get_ID() + "/terminal";

    std::string hostname = container.Get_Name_Cache() + "-" + auth.DOMN;
    std::string service = "ssh://" + container.Get_IP_Cache() + ":22";

    json ingress_conf = current_ingress;
    json ingress_rule = json::object();
    json message_body = json::object();

    // Preserve the catch-all rule (last in the array)
    json catch_all_rule = ingress_conf.back();
    ingress_conf.erase(ingress_conf.end() - 1);
    ingress_rule["hostname"] = hostname;
    ingress_rule["service"] = service;
    // ingress_rule["path"] = path;  // Uncomment if path-based routing needed

    ingress_conf.push_back(ingress_rule);
    ingress_conf.push_back(catch_all_rule);

    message_body["config"]["ingress"] = ingress_conf;
    return message_body.dump();
}

// Generate JSON config to remove an ingress rule for a container
std::string Labs_Core::Cloudflare::Generate_Remove_Ingress_Config(Container container) {
    json current_ingress = Fetch_Ingress_Config();
    return Generate_Remove_Ingress_Config(container, current_ingress);
}

// Generate JSON config to remove an ingress rule given current ingress array
std::string Labs_Core::Cloudflare::Generate_Remove_Ingress_Config(Container container, json current_ingress) {
    // std::string path = "container/" + container.Get_ID() + "/terminal"; // For path-based if needed

    std::string hostname = container.Get_Name_Cache() + "-" + auth.DOMN;
    json ingress_conf = current_ingress;
    json message_body = json::object();

    // Preserve the catch-all rule
    json catch_all_rule = ingress_conf.back();
    ingress_conf.erase(ingress_conf.end() - 1);

    // Remove ingress rules matching the container's hostname
    ingress_conf.erase(
        std::remove_if(ingress_conf.begin(), ingress_conf.end(), [&](const json& obj) {
            return obj.contains("hostname") && obj["hostname"] == hostname;
            // For path-based: && obj.contains("path") && obj["path"] == path;
            }),
        ingress_conf.end()
    );

    ingress_conf.push_back(catch_all_rule);

    message_body["config"]["ingress"] = ingress_conf;
    return message_body.dump();
}

// Generate JSON config to update an ingress rule (remove + add)
std::string Labs_Core::Cloudflare::Generate_Update_Ingress_Config(Container container) {
    json current_ingress = Fetch_Ingress_Config();
    return Generate_Update_Ingress_Config(container, current_ingress);
}

std::string Labs_Core::Cloudflare::Generate_Update_Ingress_Config(Container container, json current_ingress) {
    json removed_ingress = json::parse(Generate_Remove_Ingress_Config(container, current_ingress));
    std::string add_ingress = Generate_Add_Ingress_Config(container, removed_ingress["config"]["ingress"]);
    return add_ingress;
}


//////////////////////////////////////////////////////////////////////////
// DNS Record Management
//////////////////////////////////////////////////////////////////////////

// Fetch all proxied CNAME DNS records ending with "-<domain>"
json Labs_Core::Cloudflare::Fetch_DNS_Records() {
    std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records?type=CNAME&proxied=true&name.endswith=-" + auth.DOMN;
    std::vector<std::string> headers = {
        "Authorization: Bearer " + auth.TKN
    };
    std::string response = std::get<std::string>(curl.Get(url, headers));
    return json::parse(response);
}

// Fetch DNS record for a specific container by name
json Labs_Core::Cloudflare::Fetch_DNS_Record(Container container) {
    std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records?type=CNAME&proxied=true&name=" + container.Get_Name_Cache() + "-" + auth.DOMN;
    std::vector<std::string> headers = {
        "Authorization: Bearer " + auth.TKN
    };
    std::string response = std::get<std::string>(curl.Get(url, headers));
    return json::parse(response);
}

// Create a new DNS CNAME record for a container
int Labs_Core::Cloudflare::Create_DNS_Record(Container container) {
    std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records";
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "Authorization: Bearer " + auth.TKN
    };

    std::string data = Generate_Add_DNS_Config(container);
    std::string response = std::get<std::string>(curl.Post(url, data, headers));
    return 0;
}

// Remove DNS record for a container
int Labs_Core::Cloudflare::Remove_DNS_Record(Container container) {
    json dns_record = Fetch_DNS_Record(container);

    if (dns_record["result"].empty()) {
        if (must_cout) {
            std::cout << "No DNS record found for '" << container.Get_Name_Cache() << "'" << std::endl;
        }
        return 1; // indicate failure or no-op
    }

    std::string record_id = dns_record["result"][0]["id"];
    std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records/" + record_id;
    std::vector<std::string> headers = {
        "Authorization: Bearer " + auth.TKN
    };
    std::string response = std::get<std::string>(curl.Delete(url, headers));

    if (must_cout) {
        std::cout << "Removed record for '" << container.Get_Name_Cache() << "'" << std::endl;
        std::cout << "Record ID: '" << record_id << "'" << std::endl;
    }
    return 0;
}

// Generate JSON config to add a DNS CNAME record for a container
std::string Labs_Core::Cloudflare::Generate_Add_DNS_Config(Container container) {
    json message_body = {
        {"ttl", 3600},
        {"type", "CNAME"},
        {"proxied", true},
        {"name", container.Get_Name_Cache() + "-" + auth.DOMN},
        {"content", auth.TUNN + ".cfargotunnel.com"}
    };
    // Optional: add a base64 encoded comment
    // std::string comment = container.Get_Name_Cache() + ">" + container.Get_Owner_Cache();
    // comment = BASE64::BASE64Encode(comment);
    // message_body["comment"] = comment;

    return message_body.dump();
}
