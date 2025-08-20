#pragma once
#include "curl_wrapper.h"
#include "labs_user.h"
#include "labs_container.h"
#include "main.h"
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

using namespace Docker_Labs;
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
		Cloudflared(const API_Auth& auth);
		int Test_API();
		// Seats
		std::vector<User> Get_Seats();
		//int Revoke_Seat(User user);
		// Ingress
		json Fetch_Ingress();
		int Create_Ingress(Container container);
		//int Delete_Ingress(Container container);
		//// Appliaction
		//int Create_Application(Container container);
		//int Delete_Application(Container container);
		//// Policy
		//int Grant_Container(Container container, User user);
		//int Revoke_Container(Container container, User user);
		//int* Get_Members(Container container);
		//int* Get_Authorised_Containers(User user);
		//// Global
		//int Create(User user);
		//int Delete(Container container);
		//int Delete(User user);
	private:
		const API_Auth& auth;
		Curl_Wrapper curl;
	};

	int Test_API(const API_Auth& auth);

	std::vector<User> Get_Seats(const API_Auth& auth);

	json Fetch_Ingress(const API_Auth& auth);

}