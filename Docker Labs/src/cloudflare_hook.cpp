#include "cloudflare_hook.h"
#include <iostream>
#include <unistd.h>
#include "base64.h"

using json = nlohmann::json;


// Cloudflare::API_Auth
Docker_Labs::Cloudflare::API_Auth::API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string Domain)
	: account_id(account_id), zone_id(zone_id), tunnel_id(tunnel_id), API_token(API_token), domain(Domain)
{
}

Docker_Labs::Cloudflare::API_Auth::API_Auth(std::string account_id, std::string API_token)
	: account_id(account_id), zone_id(""), tunnel_id(""), API_token(API_token), domain("")
{
}

// Cloudflare::Cloudflared
Docker_Labs::Cloudflare::Cloudflared::Cloudflared(const API_Auth& auth)
	: auth(auth)
{
	curl = Curl_Wrapper();
};

int Docker_Labs::Cloudflare::Cloudflared::Test_API()
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/tokens/verify";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce, status;
	bool success;
	try {
		responce = curl.Get(url, headers);
	}
	catch (const char* msg) {
		return 3;
	}
	try {
		json JSON = json::parse(responce);
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

std::vector<User> Docker_Labs::Cloudflare::Cloudflared::Get_Seats()
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/access/users";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Get(url, headers);

	json seats = json::parse(responce)["result"];
	std::vector<User> users;
	for (const json& user : seats) {
		if (user["access_seat"]) {
			users.push_back(User(user.value("email", ""), user.value("uid", "")));
		}
	}

	return users;
}

json Docker_Labs::Cloudflare::Cloudflared::Fetch_Ingress() {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/cfd_tunnel/"+auth.tunnel_id+"/configurations";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Get(url, headers);

	json ingress_configuration = json::parse(responce);
	return ingress_configuration;
}
json Docker_Labs::Cloudflare::Cloudflared::Fetch_DNS_Records() {
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.zone_id + "/dns_records?type=CNAME&proxied=true&name.endswith=-" + auth.domain;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Get(url, headers);

	json records = json::parse(responce);
	return records;
}
json Docker_Labs::Cloudflare::Cloudflared::Fetch_DNS_Record(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.zone_id + "/dns_records?type=CNAME&proxied=true&name="+ container.Get_Name_Cache() + "-" + auth.domain;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Get(url, headers);

	json records = json::parse(responce);
	return records;
}
json Docker_Labs::Cloudflare::Cloudflared::Fetch_Application(Container container) {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/access/apps?exact=true&domain=" + container.Get_Name_Cache() + "-" + auth.domain;
	std::cout << url << std::endl;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Get(url, headers);

	json application = json::parse(responce);
	return application;
}

std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_Ingress_Config(Container container)
{

	std::string hostname = container.Get_Name_Cache() + "-" + auth.domain;
	std::string service = "ssh://" + container.Get_IP_Cache() + ":22";

	json current_ingress = Fetch_Ingress();
	json tunnel_conf = current_ingress["result"]["config"];
	json ingress_conf = tunnel_conf["ingress"];
	json ingress_rule = "{\"hostname\":\"\",\"service\":\"\"}"_json;
	json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	json catch_all_rule = ingress_conf.back();
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
	std::string hostname = container.Get_Name_Cache() + "-" + auth.domain;

	json current_ingress = Fetch_Ingress();
	json tunnel_conf = current_ingress["result"]["config"];
	json ingress_conf = tunnel_conf["ingress"];
	json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	json catch_all_rule = ingress_conf.back();
	ingress_conf.erase(ingress_conf.end() - 1);

	ingress_conf.erase(
		std::remove_if(ingress_conf.begin(), ingress_conf.end(), [&](const json& obj) {
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
	std::string hostname = container.Get_Name_Cache() + "-" + auth.domain;
	std::string service = "ssh://" + container.Get_IP_Cache() + ":22";

	json current_ingress = Fetch_Ingress();
	json tunnel_conf = current_ingress["result"]["config"];
	json ingress_conf = tunnel_conf["ingress"];
	json ingress_rule = "{\"hostname\":\"\",\"service\":\"\"}"_json;
	json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	json catch_all_rule = ingress_conf.back();
	ingress_conf.erase(ingress_conf.end() - 1);

	ingress_conf.erase(
		std::remove_if(ingress_conf.begin(), ingress_conf.end(), [&](const json& obj) {
			return obj.contains("hostname") && obj["hostname"] == hostname;
			}),
		ingress_conf.end()
	);

	ingress_rule["hostname"] = hostname;
	ingress_rule["service"] = service;

	ingress_conf.push_back(ingress_rule);
	ingress_conf.push_back(catch_all_rule);

	message_body["config"]["ingress"] = ingress_conf;
	return message_body.dump();
}

std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_DNS_Config(Container container)
{
	json message_body = "{\"ttl\":3600,\"type\":\"CNAME\",\"proxied\":true}"_json;
	std::string comment = container.Get_Name_Cache()+ ">" + container.Get_Owner_Cache();
	comment = BASE64::BASE64Encode(comment);
	message_body["name"] = container.Get_Name_Cache() + "-" + auth.domain;
	message_body["comment"] = comment;
	message_body["content"] = auth.tunnel_id + ".cfargotunnel.com";
	return message_body.dump();
}

std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_Application_Config(Container container) {
	json message_body = "{\"type\":\"ssh\", \"session_duration\":\"12h\",\"auto_redirect_to_identity\":true,\"allow_iframe\":true}"_json;
	message_body["domain"] = container.Get_Name_Cache() + "-" + auth.domain;
	//message_body["destinations"][0]["uri"] = container.Get_Name_Cache() + "-" + auth.domain;
	return message_body.dump();
}

std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Initial_Policy_Config(Container container)
{
	json message_body = "{\"decision\":\"allow\",\"include\":[]}"_json;
	json target = "{\"email\":{}}"_json;
	target["email"]["email"] = container.Get_Owner_Cache();
	message_body["include"].push_back(target);
	message_body["name"] = "Access Policy for '" + container.Get_Name_Cache() + "'";

	return message_body.dump();
}



// Cloudflare Floating
int Docker_Labs::Cloudflare::Test_API(const API_Auth& auth) {
	return Cloudflared(auth).Test_API();
}
std::vector<User> Docker_Labs::Cloudflare::Get_Seats(const API_Auth& auth) {
	return Cloudflared(auth).Get_Seats();
}

json Docker_Labs::Cloudflare::Fetch_Ingress(const API_Auth& auth)
{
	return Cloudflared(auth).Fetch_Ingress();
}

json Docker_Labs::Cloudflare::Fetch_DNS_Records(const API_Auth& auth) {
	return Cloudflared(auth).Fetch_DNS_Records();
}

int Docker_Labs::Cloudflare::Cloudflared::Create_Ingress(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/cfd_tunnel/" + auth.tunnel_id + "/configurations";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.API_token
	};

	std::string data = Generate_Add_Ingress_Config(container);
	std::string responce = curl.Put(url, data, headers);
	return 0;
}

int Docker_Labs::Cloudflare::Cloudflared::Remove_Ingress(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/cfd_tunnel/" + auth.tunnel_id + "/configurations";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.API_token
	};

	std::string data = Generate_Remove_Ingress_Config(container);
	std::string responce = curl.Put(url, data, headers);
	return 0;
}

int Docker_Labs::Cloudflare::Cloudflared::Update_Ingress(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/cfd_tunnel/" + auth.tunnel_id + "/configurations";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.API_token
	};

	std::string data = Generate_Update_Ingress_Config(container);
	std::string responce = curl.Put(url, data, headers);
	return 0;
}


int Docker_Labs::Cloudflare::Cloudflared::Create_DNS_Record(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.zone_id + "/dns_records";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.API_token
	};

	std::string data = Generate_Add_DNS_Config(container);
	std::string responce = curl.Post(url, data, headers);
	return 0;
}

int Docker_Labs::Cloudflare::Cloudflared::Remove_DNS_Record(Container container)
{
	json dns_record = Fetch_DNS_Record(container);
	std::string record_id = dns_record["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/zones/" + auth.zone_id + "/dns_records/" + record_id;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Delete(url, headers);
	return 0;
}


int Docker_Labs::Cloudflare::Cloudflared::Create_Application(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/access/apps";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.API_token
	};

	std::string data = Generate_Add_Application_Config(container);
	std::string responce = curl.Post(url, data, headers);
	return 0;
}

int Docker_Labs::Cloudflare::Cloudflared::Remove_Application(Container container)
{
	json application = Cloudflared::Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/access/apps/" + application_id;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Delete(url, headers);
	return 0;
}

int Docker_Labs::Cloudflare::Cloudflared::Initialize_Policy(Container container)
{
	json application = Cloudflared::Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/access/apps/" + application_id + "/policies";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.API_token
	};
	std::string data = Generate_Initial_Policy_Config(container);
	std::string responce = curl.Post(url, data, headers);

	return 0;
}

int Docker_Labs::Cloudflare::Cloudflared::Grant_Container(Container container, User user)
{
	return 0;
}





