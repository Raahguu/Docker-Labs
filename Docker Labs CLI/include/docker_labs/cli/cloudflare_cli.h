#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/labs_container.h"
/// <summary>
/// test
/// </summary>
namespace Docker_Labs::Labs_CLI::Cloudflare {

	/// <summary>
	/// Retrieves Cloudflare API authentication with a connection string piped into stdin.
	/// </summary>
	/// <returns>Cloudflare API credential object.</returns>
	Labs_Core::Cloudflare::API_Auth Get_Auth();

	/// <summary>
	/// Retrieves Cloudflare API authentication credentials with a credential file piped int stdin.
	/// </summary>
	/// <returns>Cloudflare API credential object.</returns>
	Labs_Core::Cloudflare::API_Auth Get_Auth_Legacy();

	/// <summary>
	/// Entry point for interpreting CLI commands related to Cloudflare.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Fetches user seat info from the Cloudflare API.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <returns>Status code.</returns>
	int Fetch_Seats(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Fetches existing ingress rules from the Cloudflare API.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <returns>Status code.</returns>
	int Fetch_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Fetches DNS records from the Cloudflare API.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <returns>Status code.</returns>
	int Fetch_DNS_Records(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Parses CLI arguments into a container data structure.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Parsed container object.</returns>
	Labs_Core::Container Spec_Container(int argc, char* argv[]);

	/// <summary>
	/// Parses CLI arguments into one or more user data structures.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Vector of parsed user objects.</returns>
	std::vector<Labs_Core::User> Spec_User(int argc, char* argv[]);

	/// <summary>
	/// Parses seat information using Cloudflare authentication and CLI arguments.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Vector of parsed user seat objects.</returns>
	std::vector<Labs_Core::User_Seat> Spec_Seat(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Parses CLI arguments into a container and user pair.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Tuple containing a container and a user object.</returns>
	std::tuple<Labs_Core::Container, Labs_Core::User> Spec_Container_User(int argc, char* argv[]);

	/// <summary>
	/// Creates a new ingress configuration in Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Create_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Updates an existing ingress configuration in Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Update_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Removes an ingress configuration from Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Remove_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Creates a new DNS record in Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Create_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Removes a DNS record from Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Remove_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Creates a new Cloudflare application.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Create_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Removes a Cloudflare application.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Remove_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Grants container access in Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Grant_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Revokes container access in Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Revoke_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Deactivates user seats in Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API credential object.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Deactivate_Seats(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	/// <summary>
	/// Displays help or usage information for Cloudflare CLI commands.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Help_Message(int argc, char* argv[]);
}