#include "curl_wrapper.h"
#include <string>

namespace Docker_Labs::Cloudflare {

	class Cloudflared;

	class API_Auth {
			friend class Cloudflared;
		public:
			API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token);
			API_Auth(std::string account_id, std::string API_token);
			std::string Get_Account();
		private:
			const std::string account_id;
			const std::string zone_id;
			const std::string tunnel_id;
			const std::string API_token;
			
		};

	class Cloudflared {
	public:
		Cloudflared(const API_Auth& au);
		std::string Test_API();
	private:
		const API_Auth& auth;
		Curl_Wrapper curl;
	};

	std::string Test_API(const API_Auth& auth);
}