#pragma once
#include <curl/curl.h>
#include "curl_wrapper.h"
#include "labs_user.h"
#include "labs_container.h"
#include "json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Labs_Core {

	class Cloudflare {
	public:
		class API_Auth {
			friend class Cloudflare;
		public:
			API_Auth();
			API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string domain);
		private:
			static std::string Cin();
			const std::string ACC;
			const std::string ZONE;
			const std::string TUNN;
			const std::string TKN;
			const std::string DOMN;
		};

		static int Test_API(const Cloudflare::API_Auth& auth);

		static std::vector<User> Fetch_Seats(const Cloudflare::API_Auth& auth);

		static json Fetch_Ingress(const Cloudflare::API_Auth& auth);
		static json Fetch_DNS_Records(const Cloudflare::API_Auth& auth);

		Cloudflare(const API_Auth& auth);
		Cloudflare(const API_Auth& auth, bool must_cout);
		int Test_API();
		std::vector<std::tuple<int, std::string>> Get_Return_Info(json responce);

		// Seats
		std::vector<User> Fetch_Seats();
		//int Revoke_Seat(User user);
		// Ingress
		json Fetch_Ingress();
		json Fetch_Ingress_Config();
		int Create_Ingress(Container container);
		int Remove_Ingress(Container container);
		int Update_Ingress(Container container);
		// DNS
		json Fetch_DNS_Records();
		json Fetch_DNS_Record(Container container);
		int Create_DNS_Record(Container container);
		int Remove_DNS_Record(Container container);
		// Appliaction
		json Fetch_Application(Container container);
		int Create_Application(Container container);
		int Create_Application(Container container, std::string name);
		int Remove_Application(Container container);
		//// Policy
		json Fetch_Application_Policy(Container container);
		json Fetch_Application_Policy(Container container, std::string application_id);
		int Initialize_Policy(Container container, User user);
		int Initialize_Policy(Container container, User user, std::string application_id);
		int Grant_Container(Container container, User user);
		int Revoke_Container(Container container, User user);
		//int* Get_Members(Container container);
		//int* Get_Authorised_Containers(User user);
		// Global
		int Activate_Container(Container container, User user);
		int Deactivate_Container(Container container);
		int Deactivate_Container(Container container, bool keep_container);
		//int Delete(Container container);
		//int Delete(User user);
	private:
		const API_Auth& auth;
		Curl_Wrapper curl;

		bool must_cout;

		// Ingress data
		std::string Generate_Add_Ingress_Config(Container container);
		std::string Generate_Add_Ingress_Config(Container container, json current_ingress);
		std::string Generate_Remove_Ingress_Config(Container container);
		std::string Generate_Remove_Ingress_Config(Container container, json current_ingress);
		std::string Generate_Update_Ingress_Config(Container container);
		std::string Generate_Update_Ingress_Config(Container container, json current_ingress);

		// DNS data
		std::string Generate_Add_DNS_Config(Container container);


		// Application data
		std::string Generate_Add_Application_Config(Container container, std::string name);

		// Policy data
		std::string Generate_Initial_Policy_Config(Container container, User user);
		std::string Generate_Grant_Policy_Config(Container container, User user, json application_policy);
		std::string Generate_Revoke_Policy_Config(Container container, User user, json application_policy);

	};
}
