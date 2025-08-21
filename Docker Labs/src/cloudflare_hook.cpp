#include "cloudflare_hook.h"
#include <iostream>
#include <unistd.h>

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

std::string Docker_Labs::Cloudflare::Cloudflared::Generate_Add_Ingress_Message(Container container) {
	json current_ingress = Fetch_Ingress();
	json tunnel_conf = current_ingress["result"]["config"];
	json ingress_conf = tunnel_conf["ingress"];

	json ingress_rule = "{\"hostname\":\"\",\"service\":\"\"}"_json;
	json message_body = "{\"config\": { } }"_json;

	// Save catch all rule, must be last
	json catch_all_rule = ingress_conf.back();


	ingress_conf.erase(ingress_conf.end() - 1);

	std::string hostname = container.Get_Name_Cache() + "." + auth.domain;
	std::string service = "ssh://" + container.Get_IP_Cache() + ":22";

	ingress_rule["hostname"] = hostname;
	ingress_rule["service"] = service;

	ingress_conf.push_back(ingress_rule);
	ingress_conf.push_back(catch_all_rule);

	message_body["config"]["ingress"] = ingress_conf;
	return message_body.dump();
};



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

int Docker_Labs::Cloudflare::Cloudflared::Create_Ingress(Container container)
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/cfd_tunnel/" + auth.tunnel_id + "/configurations";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.API_token
	};

	std::string data = Generate_Add_Ingress_Message(container);
	//std::string data = "{\"config\":{\"ingress\":[{\"hostname\":\"l2l-test4a_cassa.endpoints.live\",\"originRequest\":{},\"service\":\"ssh://127.0.0.1:22\"},{\"hostname\":\"v1-laith_striegher-cassa_au.endpoints.live\",\"options\":{\"require_authenticated_jwt\":true},\"service\":\"ssh://172.17.0.2:22\"},{\"hostname\":\"test_container_domain_com_e2d.labs.endpoints.live\",\"service\":\"ssh://127.0.0.1:22\"},{\"service\":\"http_status:404\"}]}}";
	std::cout << data << std::endl;
	std::string responce = curl.Put(url, data, headers);
	std::cout << responce << std::endl;
	return 0;
}


