#pragma once
#include "curl_wrapper.h"
#include "labs_user.h"
#include "labs_container.h"
#include "main.h"
#include <string>
#include "nlohmann/json.hpp"

namespace Docker_Labs::Cloudflare {

	class Cloudflared;

	class API_Auth {
			friend class Cloudflared;
		public:
			API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string domain);
			API_Auth(std::string account_id, std::string API_token);
		private:
			const std::string account_id;
			const std::string zone_id;
			const std::string tunnel_id;
			const std::string API_token;
			const std::string domain;
		};

	class Cloudflared {
	public:
		Cloudflared(const API_Auth& auth);
		int Test_API();
		// Seats
		std::vector<Docker_Labs::User> Get_Seats();
		//int Revoke_Seat(User user);
		// Ingress
		nlohmann::json Fetch_Ingress();
		int Create_Ingress(Docker_Labs::Container container);
		int Remove_Ingress(Docker_Labs::Container container);
		int Update_Ingress(Docker_Labs::Container container);
		// DNS
		nlohmann::json Fetch_DNS_Records();
		nlohmann::json Fetch_DNS_Record(Docker_Labs::Container container);
		int Create_DNS_Record(Docker_Labs::Container container);
		int Remove_DNS_Record(Docker_Labs::Container container);
		// Appliaction
		nlohmann::json Fetch_Application(Docker_Labs::Container container);
		int Create_Application(Docker_Labs::Container container);
		int Remove_Application(Docker_Labs::Container container);
		//// Policy
		nlohmann::json Fetch_Application_Policy(Docker_Labs::Container container);
		int Initialize_Policy(Docker_Labs::Container container);
		int Grant_Container(Docker_Labs::Container container, Docker_Labs::User user);
		int Revoke_Container(Docker_Labs::Container container, Docker_Labs::User user);
		//int* Get_Members(Container container);
		//int* Get_Authorised_Containers(User user);
		// Global
		int Init_Access(Container container, User user);
		//int Delete(Container container);
		//int Delete(User user);
	private:
		const API_Auth& auth;
		Curl_Wrapper curl;

		// Ingress data
		std::string Generate_Add_Ingress_Config(Docker_Labs::Container container);
		std::string Generate_Remove_Ingress_Config(Docker_Labs::Container container);
		std::string Generate_Update_Ingress_Config(Docker_Labs::Container container);

		// DNS data
		std::string Generate_Add_DNS_Config(Docker_Labs::Container container);

		// Application data
		std::string Generate_Add_Application_Config(Docker_Labs::Container container);

		// Policy data
		std::string Generate_Initial_Policy_Config(Docker_Labs::Container container);
		std::string Generate_Grant_Policy_Config(Docker_Labs::Container container, Docker_Labs::User user);
		std::string Generate_Revoke_Policy_Config(Docker_Labs::Container container, Docker_Labs::User user);

	};

	int Test_API(const API_Auth& auth);

	std::vector<Docker_Labs::User> Get_Seats(const API_Auth& auth);

	nlohmann::json Fetch_Ingress(const API_Auth& auth);
	nlohmann::json Fetch_DNS_Records(const API_Auth& auth);
}