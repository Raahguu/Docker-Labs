#include "cloudflare_hook.h"
#include "docker_hook.h"
#include <iostream>
#include <unistd.h>

// Cloudflare::API_Auth

Docker_Labs::Cloudflare::API_Auth::API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string Domain)
	: ACC(account_id), ZONE(zone_id), TUNN(tunnel_id), TKN(API_token), DOMN(Domain)
{
}
Docker_Labs::Cloudflare::API_Auth::API_Auth()
	: ACC(Cin()), ZONE(Cin()), TUNN(Cin()), TKN(Cin()), DOMN(Cin())
{
}
std::string Docker_Labs::Cloudflare::API_Auth::Cin()
{
	std::string input;
	std::cin >> input;
	return input;
}
// Obsolete

Docker_Labs::Cloudflare::API_Auth Docker_Labs::Cloudflare::API_Auth::Get_Auth()
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

    Docker_Labs::Cloudflare::API_Auth cf_auth = Docker_Labs::Cloudflare::API_Auth(
        ACC,
        ZONE,
        TUNN,
        TKN,
        DOMN
    );

	return cf_auth;
}

// Cloudflare::Cloudflared
Docker_Labs::Cloudflare::Cloudflared::Cloudflared(const API_Auth& auth)
	: auth(auth), curl(Curl_Wrapper()), must_cout(true)
{}
Docker_Labs::Cloudflare::Cloudflared::Cloudflared(const API_Auth & auth, bool must_cout)
	: auth(auth), curl(Curl_Wrapper()), must_cout(must_cout)
{
}
;


// Class mode fetching
std::vector<Docker_Labs::User> Docker_Labs::Cloudflare::Cloudflared::Fetch_Seats()
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/users";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	nlohmann::json seats = nlohmann::json::parse(responce)["result"];
	std::vector<User> users;
	for (const nlohmann::json& user : seats) {
		if (user["access_seat"]) {
			users.push_back(User(user.value("email", "")));
		}
	}

	return users;
}
nlohmann::json Docker_Labs::Cloudflare::Cloudflared::Fetch_Ingress() {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/cfd_tunnel/"+auth.TUNN+"/configurations";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	nlohmann::json ingress_configuration = nlohmann::json::parse(responce);
	return ingress_configuration;
}
nlohmann::json Docker_Labs::Cloudflare::Cloudflared::Fetch_Ingress_Config()
{
	return Fetch_Ingress()["result"]["config"]["ingress"];
}
nlohmann::json Docker_Labs::Cloudflare::Cloudflared::Fetch_DNS_Records() {
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records?type=CNAME&proxied=true&name.endswith=-" + auth.DOMN;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	nlohmann::json records = nlohmann::json::parse(responce);
	return records;
}
nlohmann::json Docker_Labs::Cloudflare::Cloudflared::Fetch_DNS_Record(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.ZONE + "/dns_records?type=CNAME&proxied=true&name=" + container.Get_Name_Cache() + "-" + auth.DOMN;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	nlohmann::json records = nlohmann::json::parse(responce);
	return records;
}
nlohmann::json Docker_Labs::Cloudflare::Cloudflared::Fetch_Application(Container container) {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps?exact=true&domain=" + container.Get_Name_Cache() + "-" + auth.DOMN;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	nlohmann::json application = nlohmann::json::parse(responce);
	return application;
}
nlohmann::json Docker_Labs::Cloudflare::Cloudflared::Fetch_Application_Policy(Container container) {
	nlohmann::json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];

	return Fetch_Application_Policy(container, application_id);
}
nlohmann::json Docker_Labs::Cloudflare::Cloudflared::Fetch_Application_Policy(Docker_Labs::Container container, std::string application_id)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));
	nlohmann::json policies = nlohmann::json::parse(responce);
	return policies;
}

std::vector<std::tuple<int, std::string>> Docker_Labs::Cloudflare::Cloudflared::Get_Return_Info(nlohmann::json responce) {
	if (responce["success"]) {
		return { std::tuple<int, std::string>(0, "Success")};
	}
	else {
		nlohmann::json errors = responce["errors"];
		std::vector<std::tuple<int, std::string>> errors_map = {};
		for (nlohmann::json error : errors) {
			errors_map.push_back(std::tuple<int, std::string>(error["code"], error["message"]));
		}
		return errors_map;
	}
}

int Docker_Labs::Cloudflare::Cloudflared::Test_API()
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
	catch (const char* msg) {
		return 3;
	}
	try {
		nlohmann::json JSON = nlohmann::json::parse(responce);
		success = JSON["success"];
		if (success) {
			status = JSON["result"]["status"];
		}
		else {
			return 2;
		}

	}
	catch (const char* msg) {
		return 1;
	}
	if (status == "active") {
		return 0;
	}
	else {
		return 2;
	}
}


// Float mode fetching
nlohmann::json Docker_Labs::Cloudflare::Fetch_Ingress(const API_Auth& auth)
{
	return Cloudflared(auth).Fetch_Ingress();
}
nlohmann::json Docker_Labs::Cloudflare::Fetch_DNS_Records(const API_Auth& auth) {
	return Cloudflared(auth).Fetch_DNS_Records();
}
std::vector<Docker_Labs::User> Docker_Labs::Cloudflare::Fetch_Seats(const API_Auth& auth) {
	return Cloudflared(auth).Fetch_Seats();
}

int Docker_Labs::Cloudflare::Test_API(const API_Auth& auth) {
	return Cloudflared(auth).Test_API();
}

// Class mode request generation
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_Ingress_Config(Container container)
{
	nlohmann::json current_ingress = Fetch_Ingress_Config();
	return Generate_Add_Ingress_Config(container, current_ingress);
}
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_Ingress_Config(Container container, json current_ingress)
{
	std::string hostname = container.Get_Name_Cache() + "-" + auth.DOMN;
	std::string service = "ssh://" + container.Get_IP_Cache() + ":22";
	nlohmann::json ingress_conf = current_ingress;
	nlohmann::json ingress_rule = "{\"hostname\":\"\",\"service\":\"\"}"_json;
	nlohmann::json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	nlohmann::json catch_all_rule = ingress_conf.back();
	ingress_conf.erase(ingress_conf.end() - 1);

	ingress_rule["hostname"] = hostname;
	ingress_rule["service"] = service;

	ingress_conf.push_back(ingress_rule);
	ingress_conf.push_back(catch_all_rule);

	message_body["config"]["ingress"] = ingress_conf;
	return message_body.dump();
}
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Remove_Ingress_Config(Container container)
{
	nlohmann::json current_ingress = Fetch_Ingress_Config();
	return Generate_Remove_Ingress_Config(container, current_ingress);
	
}
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Remove_Ingress_Config(Container container, json current_ingress)
{
	std::string hostname = container.Get_Name_Cache() + "-" + auth.DOMN;
	nlohmann::json ingress_conf = current_ingress;
	nlohmann::json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	nlohmann::json catch_all_rule = ingress_conf.back();
	ingress_conf.erase(ingress_conf.end() - 1);

	ingress_conf.erase(
		std::remove_if(ingress_conf.begin(), ingress_conf.end(), [&](const nlohmann::json& obj) {
			return obj.contains("hostname") && obj["hostname"] == hostname;
			}),
		ingress_conf.end()
	);

	ingress_conf.push_back(catch_all_rule);

	message_body["config"]["ingress"] = ingress_conf;
	return message_body.dump();
}
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Update_Ingress_Config(Container container)
{

	nlohmann::json current_ingress = Fetch_Ingress_Config();
	return Generate_Update_Ingress_Config(container, current_ingress);

}
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Update_Ingress_Config(Container container, json current_ingress)
{
	nlohmann::json removed_ingress = nlohmann::json::parse(Generate_Remove_Ingress_Config(container, current_ingress));
	std::string add_ingress = Generate_Add_Ingress_Config(container, removed_ingress["config"]["ingress"]);
	return add_ingress;
}


std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_DNS_Config(Container container)
{
	nlohmann::json message_body = "{\"ttl\":3600,\"type\":\"CNAME\",\"proxied\":true}"_json;
	//std::string comment = container.Get_Name_Cache()+ ">" + container.Get_Owner_Cache();
	//comment = BASE64::BASE64Encode(comment);
	message_body["name"] = container.Get_Name_Cache() + "-" + auth.DOMN;
	//message_body["comment"] = comment;
	message_body["content"] = auth.TUNN + ".cfargotunnel.com";
	return message_body.dump();
}

std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_Application_Config(Container container, std::string name) {
	nlohmann::json message_body = "{\"type\":\"ssh\", \"session_duration\":\"12h\",\"auto_redirect_to_identity\":true,\"allow_iframe\":true}"_json;
	message_body["domain"] = container.Get_Name_Cache() + "-" + auth.DOMN;
	message_body["name"] = name;
	return message_body.dump();
}

std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Initial_Policy_Config(Container container, User user)
{
	nlohmann::json message_body = "{\"decision\":\"allow\",\"include\":[{\"email\":{}}]}"_json;
	message_body["name"] = "Access Policy for '" + container.Get_Name_Cache() + "'";
	message_body["include"][0]["email"]["email"] = user.Get_Email();
	return message_body.dump();
}
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Grant_Policy_Config(Docker_Labs::Container container, Docker_Labs::User user, nlohmann::json application_policy)
{
	nlohmann::json result = application_policy["result"];

	if (result.size() < 1) return Generate_Initial_Policy_Config(container, user);
	std::string application_policy_id = application_policy["result"][0]["id"];

	nlohmann::json message_body = "{\"decision\":\"allow\"}"_json;
	message_body["name"] = application_policy["result"][0]["name"];
	nlohmann::json granted = application_policy["result"][0]["include"];
	nlohmann::json target = "{\"email\":{}}"_json;
	target["email"]["email"] = user.Get_Email();
	granted.push_back(target);
	message_body["include"] = granted;
	return message_body.dump();
}
std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Revoke_Policy_Config(Docker_Labs::Container container, Docker_Labs::User user, nlohmann::json application_policy)
{

	nlohmann::json message_body = "{\"decision\":\"allow\"}"_json;
	message_body["name"] = application_policy["result"][0]["name"];
	nlohmann::json granted = application_policy["result"][0]["include"];

	granted.erase(
		std::remove_if(granted.begin(), granted.end(), [&](const nlohmann::json& obj) {
			return obj["email"].contains("email") && obj["email"]["email"] == user.Get_Email();
			}),
		granted.end()
	);

	message_body["include"] = granted;
	return message_body.dump();

}


// Class mode writes
int Docker_Labs::Cloudflare::Cloudflared::Create_Ingress(Container container)
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
int Docker_Labs::Cloudflare::Cloudflared::Remove_Ingress(Container container)
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
int Docker_Labs::Cloudflare::Cloudflared::Update_Ingress(Container container)
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

int Docker_Labs::Cloudflare::Cloudflared::Create_DNS_Record(Container container)
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
int Docker_Labs::Cloudflare::Cloudflared::Remove_DNS_Record(Container container)
{
	nlohmann::json dns_record = Fetch_DNS_Record(container);

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

int Docker_Labs::Cloudflare::Cloudflared::Create_Application(Container container) {
	return Create_Application(container, container.Get_Name_Cache());
}

int Docker_Labs::Cloudflare::Cloudflared::Create_Application(Container container, std::string name)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Add_Application_Config(container, name);
	std::string responce = std::get<std::string>(curl.Post(url, data, headers));

	nlohmann::json parsed_responce = nlohmann::json::parse(responce);
	if (must_cout) {
		std::cout << "Created application for '" + container.Get_Name_Cache() + "'" << std::endl;
	}
	return 0;
}
int Docker_Labs::Cloudflare::Cloudflared::Remove_Application(Container container)
{
	nlohmann::json application = Cloudflared::Fetch_Application(container);
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

int Docker_Labs::Cloudflare::Cloudflared::Initialize_Policy(Container container, User user)
{
	nlohmann::json application = Cloudflared::Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	return Initialize_Policy(container, user, application_id);
}

int Docker_Labs::Cloudflare::Cloudflared::Initialize_Policy(Container container, User user, std::string application_id) {
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

int Docker_Labs::Cloudflare::Cloudflared::Grant_Container(Container container, User user)
{
	nlohmann::json application = Cloudflared::Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];

	nlohmann::json application_policy = Cloudflared::Fetch_Application_Policy(container, application_id);

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
int Docker_Labs::Cloudflare::Cloudflared::Revoke_Container(Container container, User user)
{
	nlohmann::json application = Cloudflared::Fetch_Application(container);
	std::string application_id = application["result"] [0] ["id"] ;
	nlohmann::json application_policy = Cloudflared::Fetch_Application_Policy(container, application_id);
	std::string application_policy_id = application_policy["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies/" + application_policy_id;
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};
	std::string data = Generate_Revoke_Policy_Config(container, user, application_policy);
	std::string responce = std::get<std::string>(curl.Put(url, data, headers));

	if (std::get<int>(Get_Return_Info(responce)[0]) && must_cout) {
		std::cout << "Revoked access to '" + user.Get_Email() + "' for container '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
	} else {
		std::cout << "Failed to revoke access to '" + user.Get_Email() + "' for container '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
		for (std::tuple<int, std::string> err : Get_Return_Info(responce)) {
			std::cout << "Code: " << std::get<int>(err) << "; Message: " << std::get<std::string>(err) << ";" << std::endl;
		}
	}
	return std::get<int>(Get_Return_Info(responce)[0]);
}


int Docker_Labs::Cloudflare::Cloudflared::Init_Access(Container container, User user)
{
	Docker_Labs::Docker::Docker docker = Docker_Labs::Docker::Docker();
	bool err_count = 0; // Let bool alg apply :)
	docker.Start(container);
	sleep(1);
	container.Cache_Update();
	std::cout << "Fetched container information." << std::endl;
	std::cout << "Deploying to cloudflare..." << std::endl;
	err_count += Create_Ingress(container);
	std::cout << " - Created ingress rule." << std::endl;
	err_count += Create_DNS_Record(container);
	std::cout << " - Created DNS record." << std::endl;
	std::string app_name = "SSH Application for '" +user.Get_Email() + "' (" + container.Get_Name_Cache().substr(container.Get_Name_Cache().length() - 3) +")";
	err_count += Create_Application(container, app_name);
	std::cout << " - Created Access application." << std::endl;
	err_count += Initialize_Policy(container, user);
	std::cout << " - Assigned default Access policy." << std::endl;
	std::cout << " - Granted access to '" << user.Get_Email() << "'" << std::endl;
	std::cout << "Finished. Access container from https://" + container.Get_Name_Cache() + "-" + auth.DOMN << std::endl;
	return err_count;
}


