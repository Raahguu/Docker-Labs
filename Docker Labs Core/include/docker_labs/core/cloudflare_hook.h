#pragma once
#include "docker_labs/core/labs_user.h"
#include "docker_labs/core/labs_container.h"
#include "docker_labs/core/curl_wrapper.h"
#include "json/json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Labs_Core {

	/// <summary>
	/// Provides methods for managing Cloudflare tunnels, DNS, applications, and access policies.
	/// Wraps Cloudflare API calls for managing containers and users within Docker Labs.
	/// </summary>
	class Cloudflare {
	public:

		/// <summary>
		/// Holds Cloudflare API credentials and identifiers.
		/// </summary>
		class API_Auth {
			friend class Cloudflare;

		public:
			/// <summary>
			/// Initializes a new instance of the <see cref="API_Auth"/> class.
			/// </summary>
			/// <param name="account_id">Cloudflare account ID.</param>
			/// <param name="zone_id">Cloudflare zone ID.</param>
			/// <param name="tunnel_id">Cloudflare tunnel ID.</param>
			/// <param name="API_token">Cloudflare API token.</param>
			/// <param name="domain">Domain name.</param>
			API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string domain);

			/// <summary>
			/// Generates a connection string from the API credentials.
			/// </summary>
			/// <returns>Connection string.</returns>
			std::string Generate_Connection_String();

			/// <summary>
			/// Creates an <see cref="API_Auth"/> instance from a connection string.
			/// </summary>
			/// <param name="connection_string">The connection string.</param>
			/// <returns>Cloudflare API credential object.</returns>
			static API_Auth From_Connection_String(const std::string& connection_string);

		private:
			/// <summary>Cloudflare account ID.</summary>
			const std::string ACC;
			/// <summary>Cloudflare zone ID.</summary>
			const std::string ZONE;
			/// <summary>Cloudflare tunnel ID.</summary>
			const std::string TUNN;
			/// <summary>Cloudflare API token.</summary>
			const std::string TKN;
			/// <summary>Domain name.</summary>
			const std::string DOMN;
		};

		/// <summary>
		/// Initializes a new instance of the <see cref="Cloudflare"/> class.
		/// </summary>
		/// <param name="auth">Cloudflare API authentication object.</param>
		Cloudflare(const API_Auth& auth);

		/// <summary>
		/// Initializes a new instance of the <see cref="Cloudflare"/> class with optional output.
		/// </summary>
		/// <param name="auth">Cloudflare API authentication object.</param>
		/// <param name="must_cout">Whether to output status to console.</param>
		Cloudflare(const API_Auth& auth, bool must_cout);

		/// <summary>
		/// Tests Cloudflare API connectivity using the provided authentication.
		/// </summary>
		/// <param name="auth">Cloudflare API authentication object.</param>
		/// <returns>API test result as a string.</returns>
		static std::string Test_API(const API_Auth& auth);

		/// <summary>
		/// Tests Cloudflare API connectivity using the current authentication.
		/// </summary>
		/// <returns>API test result as a string.</returns>
		std::string Test_API();

		/// <summary>
		/// Extracts return information from a Cloudflare API response.
		/// </summary>
		/// <param name="response">JSON response from the API.</param>
		/// <returns>Vector of tuples containing status code and message.</returns>
		std::vector<std::tuple<int, std::string>> Get_Return_Info(json response);

		/// <summary>
		/// Fetches all user seats from Cloudflare.
		/// </summary>
		/// <returns>Vector of user seat objects.</returns>
		std::vector<User_Seat> Fetch_Seats();

		/// <summary>
		/// Fetches all seated users from Cloudflare.
		/// </summary>
		/// <returns>Vector of user objects.</returns>
		std::vector<User> Fetch_Seated();

		/// <summary>
		/// Fetches seat information for a specific user.
		/// </summary>
		/// <param name="user">The user object.</param>
		/// <returns>User seat object.</returns>
		User_Seat Fetch_Seat(User user);

		/// <summary>
		/// Deactivates a user seat in Cloudflare.
		/// </summary>
		/// <param name="user">The user seat object.</param>
		/// <returns>Status code.</returns>
		int Deactivate_Seat(User_Seat user);

		/// <summary>
		/// Fetches all ingress rules from Cloudflare.
		/// </summary>
		/// <returns>JSON object containing ingress rules.</returns>
		json Fetch_Ingress();

		/// <summary>
		/// Fetches the current ingress configuration from Cloudflare.
		/// </summary>
		/// <returns>JSON object containing ingress configuration.</returns>
		json Fetch_Ingress_Config();

		/// <summary>
		/// Creates a new ingress rule for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Create_Ingress(Container container);

		/// <summary>
		/// Removes an ingress rule for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Remove_Ingress(Container container);

		/// <summary>
		/// Updates an ingress rule for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Update_Ingress(Container container);

		/// <summary>
		/// Fetches all DNS records from Cloudflare.
		/// </summary>
		/// <returns>JSON object containing DNS records.</returns>
		json Fetch_DNS_Records();

		/// <summary>
		/// Fetches the DNS record for a specific container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>JSON object containing the DNS record.</returns>
		json Fetch_DNS_Record(Container container);

		/// <summary>
		/// Creates a DNS record for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Create_DNS_Record(Container container);

		/// <summary>
		/// Removes a DNS record for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Remove_DNS_Record(Container container);

		/// <summary>
		/// Fetches a Zero Trust application for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>JSON object containing application information.</returns>
		json Fetch_Application(Container container);

		/// <summary>
		/// Creates a Zero Trust application for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Create_Application(Container container);

		/// <summary>
		/// Creates a Zero Trust application for a container with a specific name.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="name">The application name.</param>
		/// <returns>Status code.</returns>
		int Create_Application(Container container, std::string name);

		/// <summary>
		/// Removes a Zero Trust application for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Remove_Application(Container container);

		/// <summary>
		/// Fetches the application policy for a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>JSON object containing the application policy.</returns>
		json Fetch_Application_Policy(Container container);

		/// <summary>
		/// Fetches the application policy for a container and application ID.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="application_id">The application ID.</param>
		/// <returns>JSON object containing the application policy.</returns>
		json Fetch_Application_Policy(Container container, std::string application_id);

		/// <summary>
		/// Initializes an application policy for a container and user.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <returns>Status code.</returns>
		int Initialize_Policy(Container container, User user);

		/// <summary>
		/// Initializes an application policy for a container, user, and application ID.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <param name="application_id">The application ID.</param>
		/// <returns>Status code.</returns>
		int Initialize_Policy(Container container, User user, std::string application_id);

		/// <summary>
		/// Grants a user access to a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <returns>Status code.</returns>
		int Grant_Container(Container container, User user);

		/// <summary>
		/// Revokes a user's access to a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <returns>Status code.</returns>
		int Revoke_Container(Container container, User user);

		/// <summary>
		/// Activates a container for a user.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <returns>Status code.</returns>
		int Activate_Container(Container container, User user);

		/// <summary>
		/// Deactivates a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Status code.</returns>
		int Deactivate_Container(Container container);

		/// <summary>
		/// Deactivates a container, with an option to keep the container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="keep_container">Whether to keep the container after deactivation.</param>
		/// <returns>Status code.</returns>
		int Deactivate_Container(Container container, bool keep_container);

	private:
		/// <summary>
		/// The Cloudflare API authentication object.
		/// </summary>
		const API_Auth& auth;

		/// <summary>
		/// Curl wrapper for HTTP requests.
		/// </summary>
		Labs_Core::Curl_Wrapper curl;

		/// <summary>
		/// Indicates whether to output status to the console.
		/// </summary>
		bool must_cout;

		/// <summary>
		/// Generates ingress configuration for adding a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Ingress configuration string.</returns>
		std::string Generate_Add_Ingress_Config(Container container);

		/// <summary>
		/// Generates ingress configuration for adding a container, using current ingress.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="current_ingress">Current ingress JSON.</param>
		/// <returns>Ingress configuration string.</returns>
		std::string Generate_Add_Ingress_Config(Container container, json current_ingress);

		/// <summary>
		/// Generates ingress configuration for removing a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Ingress configuration string.</returns>
		std::string Generate_Remove_Ingress_Config(Container container);

		/// <summary>
		/// Generates ingress configuration for removing a container, using current ingress.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="current_ingress">Current ingress JSON.</param>
		/// <returns>Ingress configuration string.</returns>
		std::string Generate_Remove_Ingress_Config(Container container, json current_ingress);

		/// <summary>
		/// Generates ingress configuration for updating a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>Ingress configuration string.</returns>
		std::string Generate_Update_Ingress_Config(Container container);

		/// <summary>
		/// Generates ingress configuration for updating a container, using current ingress.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="current_ingress">Current ingress JSON.</param>
		/// <returns>Ingress configuration string.</returns>
		std::string Generate_Update_Ingress_Config(Container container, json current_ingress);

		/// <summary>
		/// Generates DNS configuration for adding a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <returns>DNS configuration string.</returns>
		std::string Generate_Add_DNS_Config(Container container);

		/// <summary>
		/// Generates application configuration for adding a container with a specific name.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="name">The application name.</param>
		/// <returns>Application configuration string.</returns>
		std::string Generate_Add_Application_Config(Container container, std::string name);

		/// <summary>
		/// Generates initial policy configuration for a container and user.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <returns>Policy configuration string.</returns>
		std::string Generate_Initial_Policy_Config(Container container, User user);

		/// <summary>
		/// Generates policy configuration for granting a user access to a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <param name="application_policy">Current application policy JSON.</param>
		/// <returns>Policy configuration string.</returns>
		std::string Generate_Grant_Policy_Config(Container container, User user, json application_policy);

		/// <summary>
		/// Generates policy configuration for revoking a user's access to a container.
		/// </summary>
		/// <param name="container">The container object.</param>
		/// <param name="user">The user object.</param>
		/// <param name="application_policy">Current application policy JSON.</param>
		/// <returns>Policy configuration string.</returns>
		std::string Generate_Revoke_Policy_Config(Container container, User user, json application_policy);

		/// <summary>
		/// Generates configuration for deactivating a user seat.
		/// </summary>
		/// <param name="user">The user seat object.</param>
		/// <returns>Seat deactivation configuration string.</returns>
		std::string Generate_Seat_Deactivation(User_Seat user);

		/// <summary>
		/// Generates configuration for bulk deactivation of user seats.
		/// </summary>
		/// <param name="users">Vector of user seat objects.</param>
		/// <returns>Bulk seat deactivation configuration string.</returns>
		std::string Generate_Bulk_Seat_Deactivation(std::vector<User_Seat> users);
	};

}