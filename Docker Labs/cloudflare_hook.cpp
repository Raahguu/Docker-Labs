#include "cloudflare_hook.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

using namespace std;

Docker_Labs::Cloudflare::API_Auth::API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token)
	: account_id(account_id), zone_id(zone_id), tunnel_id(tunnel_id), API_token(API_token)
{
}

Docker_Labs::Cloudflare::API_Auth::API_Auth(std::string account_id, std::string API_token)
	: account_id(account_id), zone_id(""), tunnel_id(""), API_token(API_token)
{
}

std::string Docker_Labs::Cloudflare::API_Auth::Get_Account() {
	return account_id;
};

Docker_Labs::Cloudflare::Cloudflared::Cloudflared(const API_Auth& auth)
	: auth(auth)
{
	curl = Curl_Wrapper();
};

bool Docker_Labs::Cloudflare::Cloudflared::Test_API()
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.account_id + "/tokens/verify";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.API_token
	};
	std::string responce = curl.Get(url, headers);
	bool active = json::parse(responce)["result"]["status"] == "active";
	return active;
}

bool Docker_Labs::Cloudflare::Test_API(const API_Auth& auth) { Cloudflared(auth).Test_API(); }
