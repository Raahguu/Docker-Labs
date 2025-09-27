
// ---------------------------------------------
// @file cloudflare.h
// @brief Declares the Cloudflare class for managing tunnels, DNS, applications, and access policies.
// 
// This module wraps Cloudflare API calls for managing containers and users within Docker Labs,
// providing methods for ingress configuration, DNS records, Zero Trust applications, and access control.
//
// Dependencies:
// - docker_labs/core/labs_user.h
// - docker_labs/core/labs_container.h
// - json (nlohmann::json)
// - Curl_Wrapper
// ---------------------------------------------

#pragma once
#include "docker_labs/core/labs_user.h"
#include "docker_labs/core/labs_container.h"
#include "docker_labs/core/curl_wrapper.h"
#include "json/json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Labs_Core {

	class Cloudflare {
	public:

		// ---------------------------------------------
		// @class API_Auth
		// @brief Holds Cloudflare API credentials and identifiers.
		// ---------------------------------------------
		class API_Auth {
			friend class Cloudflare;

		public:
			API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string domain);

			std::string Generate_Connection_String();
			static API_Auth From_Connection_String(const std::string& connection_string);

		private:
			const std::string ACC;
			const std::string ZONE;
			const std::string TUNN;
			const std::string TKN;
			const std::string DOMN;
		};

		// ---------------------------------------------
		// Constructors
		// ---------------------------------------------
		Cloudflare(const API_Auth& auth);
		Cloudflare(const API_Auth& auth, bool must_cout);

		// ---------------------------------------------
		// API Testing
		// ---------------------------------------------
		static std::string Test_API(const API_Auth& auth);
		std::string Test_API();

		// ---------------------------------------------
		// General Utilities
		// ---------------------------------------------
		std::vector<std::tuple<int, std::string>> Get_Return_Info(json response);

		// ---------------------------------------------
		// Seat & User Management
		// ---------------------------------------------
		std::vector<User_Seat> Fetch_Seats();
		std::vector<User> Fetch_Seated();
		User_Seat Fetch_Seat(User user);
		int Deactivate_Seat(User_Seat user);

		// ---------------------------------------------
		// Ingress Management
		// ---------------------------------------------
		json Fetch_Ingress();
		json Fetch_Ingress_Config();
		int Create_Ingress(Container container);
		int Remove_Ingress(Container container);
		int Update_Ingress(Container container);

		// ---------------------------------------------
		// DNS Management
		// ---------------------------------------------
		json Fetch_DNS_Records();
		json Fetch_DNS_Record(Container container);
		int Create_DNS_Record(Container container);
		int Remove_DNS_Record(Container container);

		// ---------------------------------------------
		// Application Management (Zero Trust)
		// ---------------------------------------------
		json Fetch_Application(Container container);
		int Create_Application(Container container);
		int Create_Application(Container container, std::string name);
		int Remove_Application(Container container);

		// ---------------------------------------------
		// Policy Management
		// ---------------------------------------------
		json Fetch_Application_Policy(Container container);
		json Fetch_Application_Policy(Container container, std::string application_id);
		int Initialize_Policy(Container container, User user);
		int Initialize_Policy(Container container, User user, std::string application_id);

		int Grant_Container(Container container, User user);
		int Revoke_Container(Container container, User user);

		// ---------------------------------------------
		// High-Level Container Activation & Deactivation
		// ---------------------------------------------
		int Activate_Container(Container container, User user);
		int Deactivate_Container(Container container);
		int Deactivate_Container(Container container, bool keep_container);

	private:
		// ---------------------------------------------
		// Internal State
		// ---------------------------------------------
		const API_Auth& auth;
		Labs_Core::Curl_Wrapper curl;
		bool must_cout;

		// ---------------------------------------------
		// Internal Generators — Ingress Config
		// ---------------------------------------------
		std::string Generate_Add_Ingress_Config(Container container);
		std::string Generate_Add_Ingress_Config(Container container, json current_ingress);
		std::string Generate_Remove_Ingress_Config(Container container);
		std::string Generate_Remove_Ingress_Config(Container container, json current_ingress);
		std::string Generate_Update_Ingress_Config(Container container);
		std::string Generate_Update_Ingress_Config(Container container, json current_ingress);

		// ---------------------------------------------
		// Internal Generators — DNS Config
		// ---------------------------------------------
		std::string Generate_Add_DNS_Config(Container container);

		// ---------------------------------------------
		// Internal Generators — Application Config
		// ---------------------------------------------
		std::string Generate_Add_Application_Config(Container container, std::string name);

		// ---------------------------------------------
		// Internal Generators — Policy Config
		// ---------------------------------------------
		std::string Generate_Initial_Policy_Config(Container container, User user);
		std::string Generate_Grant_Policy_Config(Container container, User user, json application_policy);
		std::string Generate_Revoke_Policy_Config(Container container, User user, json application_policy);

		// ---------------------------------------------
		// Internal Generators — Seat Deactivation
		// ---------------------------------------------
		std::string Generate_Seat_Deactivation(User_Seat user);
		std::string Generate_Bulk_Seat_Deactivation(std::vector<User_Seat> users);
	};

} // namespace Docker_Labs::Labs_Core
