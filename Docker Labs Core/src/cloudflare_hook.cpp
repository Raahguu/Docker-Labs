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
// Cloudflare::API_Auth

Labs_Core::Cloudflare::API_Auth::API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string Domain)
	: ACC(account_id), ZONE(zone_id), TUNN(tunnel_id), TKN(API_token), DOMN(Domain)
{
}

std::string getFirstUUID() {
	std::string first;
	bool found = false;
	for (const auto& entry : std::filesystem::directory_iterator("/dev/disk/by-uuid")) {
		std::string name = entry.path().filename().string();
		if (!found || name < first) {
			first = name;
			found = true;
		}
	}
	return found ? first : "";

}

std::string xorCipher(const std::string& input, const std::string& key) {
	std::string output = input;
	for (size_t i = 0; i < input.size(); ++i) {
		output[i] = input[i] ^ key[i % key.size()];
	}
	return output;
}

std::string Labs_Core::Cloudflare::API_Auth::Generate_Connection_String() {
	using It = boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8>>;

	std::string UUID = getFirstUUID();
	if (UUID.empty()) {
		throw "NO UUID";
	}
	std::string plaintext = ACC + ZONE + TUNN + TKN + DOMN;
	std::string ciphertext = xorCipher(plaintext, UUID);
	auto tmp = std::string(It(std::begin(ciphertext)), It(std::end(ciphertext)));
	return tmp.append((3 - ciphertext.size() % 3) % 3, '=');
}

Labs_Core::Cloudflare::API_Auth Labs_Core::Cloudflare::API_Auth::From_Connection_String(const std::string& connection_string) {
	using It = boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6>;

	std::string UUID = getFirstUUID();
	if (UUID.empty()) {
		throw std::runtime_error("NO UUID");
	}

	// Decode the Base64 connection string
	std::string ciphertext = boost::algorithm::trim_right_copy_if(std::string(It(std::begin(connection_string)), It(std::end(connection_string))), [](char c) {
		return c == '\0';
		});

	// De-obfuscate the string
	std::string plaintext = xorCipher(ciphertext, UUID);

	// Extract components based on specified lengths
	if (plaintext.size() < 140) { // 32 + 32 + 36 + 40 = 140
		throw std::runtime_error("Invalid connection string format");
	}

	std::string ACC = plaintext.substr(0, 32);
	std::string ZONE = plaintext.substr(32, 32);
	std::string TUNN = plaintext.substr(64, 36);
	std::string TKN = plaintext.substr(100, 40);
	std::string DOMN = plaintext.substr(140);

	return API_Auth(ACC, ZONE, TUNN, TKN, DOMN);
}
// Cloudflare
Labs_Core::Cloudflare::Cloudflare(const API_Auth& auth)
	: auth(auth), curl(Curl_Wrapper()), must_cout(true)
{}
Labs_Core::Cloudflare::Cloudflare(const API_Auth & auth, bool must_cout)
	: auth(auth), curl(Curl_Wrapper()), must_cout(must_cout)
{
}


// Class mode fetching
std::vector<Labs_Core::User_Seat> Labs_Core::Cloudflare::Fetch_Seats()
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/users";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));
	json seats = json::parse(responce)["result"];
	std::vector<Labs_Core::User_Seat> users;
	for (const json& user : seats) {
		users.push_back(User_Seat(user));
	}

	return users;
}
std::vector<Labs_Core::User> Labs_Core::Cloudflare::Fetch_Seated()
{
	std::vector<Labs_Core::User> users;
	for (Labs_Core::User_Seat seated_user: Fetch_Seats()) {
		users.push_back(seated_user);
	}
	return users;
}
Labs_Core::User_Seat Labs_Core::Cloudflare::Fetch_Seat(Labs_Core::User user)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/users?email=" + user.Get_Email();
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));
	json users = json::parse(responce)["result"];
	if (users.size() < 1) {
		throw "User does not have a seat.";
	}
	return User_Seat(users[0]);
}
json Labs_Core::Cloudflare::Fetch_Ingress() {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/"+auth.TUNN+"/configurations";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	json ingress_configuration = json::parse(responce);
	return ingress_configuration;
}
json Labs_Core::Cloudflare::Fetch_Ingress_Config()
{
	return Fetch_Ingress()["result"]["config"]["ingress"];
}
json Labs_Core::Cloudflare::Fetch_DNS_Records() {
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records?type=CNAME&proxied=true&name.endswith=-" + auth.DOMN;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	json records = json::parse(responce);
	return records;
}
json Labs_Core::Cloudflare::Fetch_DNS_Record(Labs_Core::Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records?type=CNAME&proxied=true&name=" + container.Get_Name_Cache() + "-" + auth.DOMN;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	json records = json::parse(responce);
	return records;
}
json Labs_Core::Cloudflare::Fetch_Application(Labs_Core::Container container) {
	std::string path = "container%2f" + container.Get_ID() + "%2fterminal";
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps?exact=true&search="+path;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	json application = json::parse(responce);
	return application;
}
json Labs_Core::Cloudflare::Fetch_Application_Policy(Labs_Core::Container container) {
	json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];

	return Fetch_Application_Policy(container, application_id);
}
json Labs_Core::Cloudflare::Fetch_Application_Policy(Labs_Core::Container container, std::string application_id)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));
	json policies = json::parse(responce);
	return policies;
}

std::vector<std::tuple<int, std::string>> Labs_Core::Cloudflare::Get_Return_Info(json responce) {
	if (responce["success"]) {
		return { std::tuple<int, std::string>(0, "Success")};
	}
	else {
		json errors = responce["errors"];
		std::vector<std::tuple<int, std::string>> errors_map = {};
		for (json error : errors) {
			errors_map.push_back(std::tuple<int, std::string>(error["code"], error["message"]));
		}
		return errors_map;
	}
}

std::string Labs_Core::Cloudflare::Test_API()
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/tokens/verify";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce, status;
	bool success;
	try {
		responce = std::get<std::string>(curl.Get(url, headers));
	}
	catch (const char*) {
		return "Failed to fetch";
	}
	try {
		json JSON = json::parse(responce);
		success = JSON["success"];
		if (success) {
			status = JSON["result"]["status"];
		}
		else {
			return auth.ACC + "\n" + auth.ZONE + "\n" + auth.TUNN + "\n" + auth.TKN + "\n" + auth.DOMN;
		}

	}
	catch (const char*) {
		return "Failed to parse";
	}
	return status;
}


// Float mode fetching
json Labs_Core::Cloudflare::Fetch_Ingress(const API_Auth& auth)
{
	return Labs_Core::Cloudflare(auth).Fetch_Ingress();
}
json Labs_Core::Cloudflare::Fetch_DNS_Records(const API_Auth& auth) {
	return Labs_Core::Cloudflare(auth).Fetch_DNS_Records();
}

std::string Labs_Core::Cloudflare::Test_API(const API_Auth& auth) {
	return Labs_Core::Cloudflare(auth).Test_API();
}

// Class mode request generation
std::string Labs_Core::Cloudflare::Generate_Add_Ingress_Config(Container container)
{
	json current_ingress = Fetch_Ingress_Config();
	return Generate_Add_Ingress_Config(container, current_ingress);
}
std::string Labs_Core::Cloudflare::Generate_Add_Ingress_Config(Container container, json current_ingress)
{
	std::string path = "container/" + container.Get_ID() + "/terminal";
	std::string service = "ssh://" + container.Get_IP_Cache() + ":22";
	json ingress_conf = current_ingress;
	json ingress_rule = "{}"_json;
	json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	json catch_all_rule = ingress_conf.back();
	ingress_conf.erase(ingress_conf.end() - 1);

	ingress_rule["hostname"] = auth.DOMN;
	ingress_rule["service"] = service;
	ingress_rule["path"] = path;

	ingress_conf.push_back(ingress_rule);
	ingress_conf.push_back(catch_all_rule);

	message_body["config"]["ingress"] = ingress_conf;
	return message_body.dump();
}
std::string Labs_Core::Cloudflare::Generate_Remove_Ingress_Config(Container container)
{
	json current_ingress = Fetch_Ingress_Config();
	return Generate_Remove_Ingress_Config(container, current_ingress);
	
}
std::string Labs_Core::Cloudflare::Generate_Remove_Ingress_Config(Container container, json current_ingress)
{
	std::string path = "container/" + container.Get_ID() + "/terminal";
	json ingress_conf = current_ingress;
	json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	json catch_all_rule = ingress_conf.back();
	ingress_conf.erase(ingress_conf.end() - 1);

	ingress_conf.erase(
		std::remove_if(ingress_conf.begin(), ingress_conf.end(), [&](const json& obj) {
			return obj.contains("hostname") && obj["hostname"] == auth.DOMN && obj.contains("path") && obj["path"] == path;
			}),
		ingress_conf.end()
	);

	ingress_conf.push_back(catch_all_rule);

	message_body["config"]["ingress"] = ingress_conf;
	return message_body.dump();
}
std::string Labs_Core::Cloudflare::Generate_Update_Ingress_Config(Container container)
{

	json current_ingress = Fetch_Ingress_Config();
	return Generate_Update_Ingress_Config(container, current_ingress);

}
std::string Labs_Core::Cloudflare::Generate_Update_Ingress_Config(Container container, json current_ingress)
{
	json removed_ingress = json::parse(Generate_Remove_Ingress_Config(container, current_ingress));
	std::string add_ingress = Generate_Add_Ingress_Config(container, removed_ingress["config"]["ingress"]);
	return add_ingress;
}


std::string Labs_Core::Cloudflare::Generate_Add_DNS_Config(Container container)
{
	json message_body = "{\"ttl\":3600,\"type\":\"CNAME\",\"proxied\":true}"_json;
	//std::string comment = container.Get_Name_Cache()+ ">" + container.Get_Owner_Cache();
	//comment = BASE64::BASE64Encode(comment);
	message_body["name"] = container.Get_Name_Cache() + "-" + auth.DOMN;
	//message_body["comment"] = comment;
	message_body["content"] = auth.TUNN + ".cfargotunnel.com";
	return message_body.dump();
}

std::string Labs_Core::Cloudflare::Generate_Add_Application_Config(Container container, std::string name) {
	std::string path = "/container/" + container.Get_ID() + "/terminal";
	json message_body = "{\"type\":\"ssh\", \"session_duration\":\"12h\",\"auto_redirect_to_identity\":true,\"allow_iframe\":true}"_json;
	message_body["domain"] = auth.DOMN + path;
	message_body["name"] = name;
	return message_body.dump();
}

std::string Labs_Core::Cloudflare::Generate_Initial_Policy_Config(Container container, User user)
{
	json message_body = "{\"decision\":\"allow\",\"include\":[{\"email\":{}}]}"_json;
	message_body["name"] = "Access Policy for '" + container.Get_Name_Cache() + "'";
	message_body["include"][0]["email"]["email"] = user.Get_Email();
	return message_body.dump();
}
std::string Labs_Core::Cloudflare::Generate_Grant_Policy_Config(Labs_Core::Container container, Labs_Core::User user, json application_policy)
{
	json result = application_policy["result"];

	if (result.size() < 1) return Labs_Core::Cloudflare::Generate_Initial_Policy_Config(container, user);
	std::string application_policy_id = application_policy["result"][0]["id"];

	json message_body = "{\"decision\":\"allow\"}"_json;
	message_body["name"] = application_policy["result"][0]["name"];
	json granted = application_policy["result"][0]["include"];
	json target = "{\"email\":{}}"_json;
	target["email"]["email"] = user.Get_Email();
	granted.push_back(target);
	message_body["include"] = granted;
	return message_body.dump();
}
std::string Labs_Core::Cloudflare::Generate_Revoke_Policy_Config(Labs_Core::Container container, Labs_Core::User user, json application_policy)
{

	json message_body = "{\"decision\":\"allow\"}"_json;
	message_body["name"] = application_policy["result"][0]["name"];
	json granted = application_policy["result"][0]["include"];

	granted.erase(
		std::remove_if(granted.begin(), granted.end(), [&](const json& obj) {
			return obj["email"].contains("email") && obj["email"]["email"] == user.Get_Email();
			}),
		granted.end()
	);

	message_body["include"] = granted;
	return message_body.dump();

}
std::string Labs_Core::Cloudflare::Generate_Seat_Deactivation(Labs_Core::User_Seat user)
{
	json message_body = "[{\"access_seat\":false,\"gateway_seat\":false}]"_json;
	message_body[0]["seat_uid"] = user.Get_SeatUID();
	return message_body.dump();
}
std::string Labs_Core::Cloudflare::Generate_Bulk_Seat_Deactivation(std::vector<Labs_Core::User_Seat> users)
{
	json message_body = "[]";
	json user_object = "";

	for (Labs_Core::User_Seat user : users) {
		user_object = "{\"access_seat\":false,\"gateway_seat\":false}"_json;
		user_object["seat_uid"] = user.Get_SeatUID();
		message_body.push_back(user_object);
	}

	return message_body.dump();
}


// Class mode writes
int Labs_Core::Cloudflare::Create_Ingress(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/" + auth.TUNN + "/configurations";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Add_Ingress_Config(container);
	std::string responce = std::get<std::string>(curl.Put(url, data, headers));

	if (must_cout) {
		std::cout << "Created rule for '" + container.Get_Name_Cache() + "'" << std::endl;
	}
	return 0;
}
int Labs_Core::Cloudflare::Remove_Ingress(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/" + auth.TUNN + "/configurations";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Remove_Ingress_Config(container);
	std::string responce = std::get<std::string>(curl.Put(url, data, headers));

	if (must_cout) {
		std::cout << "Removed rule for '" + container.Get_Name_Cache() + "'" << std::endl;
	}
	return 0;
}
int Labs_Core::Cloudflare::Update_Ingress(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/" + auth.TUNN + "/configurations";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Update_Ingress_Config(container);
	std::string responce = std::get<std::string>(curl.Put(url, data, headers));

	if (must_cout) {
		std::cout << "Updated rule for '" + container.Get_Name_Cache() + "'" << std::endl;
	}
	return 0;
}

int Labs_Core::Cloudflare::Create_DNS_Record(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Add_DNS_Config(container);
	std::string responce = std::get<std::string>(curl.Post(url, data, headers));
	return 0;
}
int Labs_Core::Cloudflare::Remove_DNS_Record(Container container)
{
	json dns_record = Fetch_DNS_Record(container);

	std::string record_id = dns_record["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records/" + record_id;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Delete(url, headers));
	std::cout << "Removed record for '" + container.Get_Name_Cache() + "'" << std::endl;
	std::cout << "Record ID: '" + record_id + "'" << std::endl;
	return 0;
}

int Labs_Core::Cloudflare::Create_Application(Container container) {
	return Create_Application(container, container.Get_Name_Cache());
}

int Labs_Core::Cloudflare::Create_Application(Container container, std::string name)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Add_Application_Config(container, name);
	std::string responce = std::get<std::string>(curl.Post(url, data, headers));

	json parsed_responce = json::parse(responce);
	if (must_cout) {
		std::cout << "Created application for '" + container.Get_Name_Cache() + "'" << std::endl;
	}
	return 0;
}
int Labs_Core::Cloudflare::Remove_Application(Container container)
{
	json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Delete(url, headers));
	if (must_cout) {
		std::cout << "Removed application for '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
	}
	return 0;
}

int Labs_Core::Cloudflare::Initialize_Policy(Container container, User user)
{
	json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	return Initialize_Policy(container, user, application_id);
}

int Labs_Core::Cloudflare::Initialize_Policy(Container container, User user, std::string application_id) {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};
	std::string data = Generate_Initial_Policy_Config(container, user);
	std::string responce = std::get<std::string>(curl.Post(url, data, headers));
	if (must_cout) {
		std::cout << "Created access policy for '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Granted access to '" + user.Get_Email() + "' for container '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
	}
	return 0;
}

int Labs_Core::Cloudflare::Grant_Container(Container container, User user)
{
	json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];

	json application_policy = Fetch_Application_Policy(container, application_id);

	if (application_policy["result"].size() < 1) {
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
		std::string responce = std::get<std::string>(curl.Put(url, data, headers));
	}

	if (must_cout) {
		std::cout << "Granted access to '" + user.Get_Email() + "' for container '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
	}
	return 0;
}
int Labs_Core::Cloudflare::Revoke_Container(Container container, User user)
{
	json application = Fetch_Application(container);
	std::string application_id = application["result"] [0] ["id"] ;
	json application_policy = Fetch_Application_Policy(container, application_id);
	std::string application_policy_id = application_policy["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies/" + application_policy_id;
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};
	std::string data = Generate_Revoke_Policy_Config(container, user, application_policy);
	std::string responce = std::get<std::string>(curl.Put(url, data, headers));

	if (std::get<0>(Get_Return_Info(json::parse(responce))[0]) && must_cout) {
		std::cout << "Revoked access to '" + user.Get_Email() + "' for container '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
	} else {
		std::cout << "Failed to revoke access to '" + user.Get_Email() + "' for container '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
		for (std::tuple<int, std::string> err : Get_Return_Info(json::parse(responce))) {
			std::cout << "Code: " << std::get<int>(err) << "; Message: " << std::get<std::string>(err) << ";" << std::endl;
		}
	}
	return std::get<0>(Get_Return_Info(json::parse(responce))[0]);
}
int Labs_Core::Cloudflare::Deactivate_Seat(User_Seat seated_user)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/seats";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Seat_Deactivation(seated_user);
	std::string responce = std::get<std::string>(curl.Patch(url, data, headers));
	return 0;
}

int Labs_Core::Cloudflare::Activate_Container(Container container, User user)
{
	Labs_Core::Docker docker = Labs_Core::Docker();
	bool err_count = 0; // Let bool alg apply :)
	docker.Start(container);
	container.Cache_Update();
	std::cout << "Fetched container information." << std::endl;
	std::cout << "Deploying to cloudflare..." << std::endl;
	err_count = err_count + Create_Ingress(container);
	std::cout << " - Created ingress rule." << std::endl;
	err_count = err_count + Create_DNS_Record(container);
	std::cout << " - Created DNS record." << std::endl;
	std::string app_name = "SSH Application for '" +user.Get_Email() + "' (" + container.Get_Name_Cache().substr(container.Get_Name_Cache().length() - 3) +")";
	err_count = err_count + Create_Application(container, app_name);
	std::cout << " - Created Access application." << std::endl;
	err_count = err_count + Initialize_Policy(container, user);
	std::cout << " - Assigned default Access policy." << std::endl;
	std::cout << " - Granted access to '" << user.Get_Email() << "'" << std::endl;
	std::cout << "Finished. Access container from https://" + container.Get_Name_Cache() + "-" + auth.DOMN << std::endl;
	return err_count;
}

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

	if (keep_container == false) {
		container.Remove();
	}
	else {
		container.Stop();
	}

	return 0;
}
