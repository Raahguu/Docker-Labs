

#include "main.h"
#include <curl/curl.h>
#include "cloudflare_hook.h"
using namespace std;
namespace Docker_Labs::Cloudflared {
	int Docker_Labs::Cloudflared::Revoke_Seat(User user)
	{
		return 0;
	}
	int Create_Ingress(Container container)
	{
		return 0;
	}
}e

namespace Docker_Labs::Cloudflare_Hook {
	class API_Auth {
	public:
		API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token) {
			this->account_id = account_id;
			this->zone_id = zone_id;
			this->tunnel_id = tunnel_id;
			this->API_token = tunnel_id;
		}
	private:
		std::string account_id;
		std::string zone_id;
		std::string tunnel_id;
		std::string API_token;
	};
}