/**
 * cloudflare_cli.h
 * Declarations for handling Cloudflare CLI commands.
 *
 * Contains function prototypes and classes related to Cloudflare
 * API authentication and command handling within the CLI.
 */


#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/labs_container.h"

namespace Docker_Labs::Labs_CLI::Cloudflare {
	// === AUTHENTICATION ===
	// Retrieves Cloudflare API authentication credentials (standard and legacy).
	Labs_Core::Cloudflare::API_Auth Get_Auth();
	Labs_Core::Cloudflare::API_Auth Get_Auth_Legacy();

	// === COMMAND HANDLING ===
	// Entry point for interpreting CLI commands related to Cloudflare.
	int Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	// === FETCH / READ OPERATIONS ===
	// Retrieves data from Cloudflare API.
	// ===============================
	// Fetches user seat info.
	int Fetch_Seats(Labs_Core::Cloudflare::API_Auth cf_auth);
	// Fetches existing ingress rules.
	int Fetch_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth);
	// Fetches DNS records.
	int Fetch_DNS_Records(Labs_Core::Cloudflare::API_Auth cf_auth);

	// === CLI OBJECT PARSERS ===
	// Parses CLI arguments into internal data structures.
	// ==========================
	// Parses a container.
	Labs_Core::Container Spec_Container(int argc, char* argv[]);
	// Parses one or more users.
	std::vector<Labs_Core::User> Spec_User(int argc, char* argv[]);
	// Parses seat with information using Cloudflare.
	std::vector<Labs_Core::User_Seat> Spec_Seat(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	// Parses a container and user pair.
	std::tuple<Labs_Core::Container, Labs_Core::User> Spec_Container_User(int argc, char* argv[]);

	// === INGRESS MANAGEMENT ===
	// Functions to create, update, and remove ingress configurations.
	int Create_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Update_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	// === DNS MANAGEMENT ===
	// Functions to create and remove DNS records.
	int Create_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_DNS(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	// === APPLICATION MANAGEMENT ===
	// Functions to create and remove Cloudflare applications.
	int Create_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Application(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	// === PERMISSION MANAGEMENT ===
	// Grant or revoke container access, or deactivate user seats.
	int Grant_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Revoke_Container(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);
	int Deactivate_Seats(Labs_Core::Cloudflare::API_Auth cf_auth, int argc, char* argv[]);

	// === MISCELLANEOUS ===
	// Displays help or usage information.
	int Help_Message(int argc, char* argv[]);
}
