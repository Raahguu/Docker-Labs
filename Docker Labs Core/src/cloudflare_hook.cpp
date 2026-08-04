// -----------------------------------------------------------------------------
// @file cloudflare_auth_and_seats.cpp
// @brief Implementation of Cloudflare API authentication, connection string 
// encoding/decoding, and user seat management for Docker Labs.
//
// This file provides mechanisms to encode/decode Cloudflare API credentials 
// into a connection string secured by XOR obfuscation using system UUID, 
// and manages fetching user seats from Cloudflare Access.
// -----------------------------------------------------------------------------


#include <tuple>
#include <iostream>
#include <filesystem> // Used to read UUID from system
#include <algorithm>  // for std::remove_if
#include <string>
#include <cstdlib> // For getenv()
#include <sstream> // For the spliting using the stringstream

#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"
#include "docker_labs/core/curl_wrapper.h"
#include "docker_labs/core/labs_user.h"

using namespace Docker_Labs;

// -----------------------------------------------------------------------------
// API_Auth Class: Holds Cloudflare API authentication credentials.
// -----------------------------------------------------------------------------

// Constructs an API_Auth object with the necessary tokens and IDs
Labs_Core::Cloudflare::API_Auth::API_Auth(std::string account_id, std::string zone_id, std::string tunnel_id, std::string API_token, std::string Domain)
	: ACC(account_id), ZONE(zone_id), TUNN(tunnel_id), TKN(API_token), DOMN(Domain)
{
}

// -----------------------------------------------------------------------------
// Utility Functions
// -----------------------------------------------------------------------------

std::string Labs_Core::Cloudflare::API_Auth::Generate_Connection_String() {
    std::string connection_string = ACC + "," + ZONE + "," + TUNN + "," + TKN + "," + DOMN;

    return connection_string;
}

// Gets all the cloudflare environment variable values an API_Auth object.
Labs_Core::Cloudflare::API_Auth Labs_Core::Cloudflare::API_Auth::From_Connection_String(const std::string& connection_string) {
    // split the argument into 5 parts based off the commas
    std::stringstream ss(&connection_string);

    std::string parts[5];

    for (int8_t i = 0; i < 5; i++)
    {
        std::getline(ss, parts[i], ",");
    }
    
	// Extract each credential by substring offsets
	std::string ACC = parts[0];
	std::string ZONE = parts[1];
	std::string TUNN = parts[2];
	std::string TKN = parts[3];
	std::string DOMN = parts[4];

	return API_Auth(ACC, ZONE, TUNN, TKN, DOMN);
}

Labs_Core::Cloudflare::API_Auth Labs_Core::Cloudflare::API_Auth::From_Env() {
	const char* env_var_name = "DOCKER_LABS_CONN_STR";

	const char* env_var_value_cstr = std::getenv(env_var_name);

	if (env_var_value_cstr == nullptr) {
		throw std::runtime_error("Environment variable " + std::string(env_var_name) + " not set");
	}
	std::string connection_string(env_var_value_cstr);
	return From_Connection_String(connection_string);
}

// -----------------------------------------------------------------------------
// Cloudflare Class: Core API interaction methods
// -----------------------------------------------------------------------------

// Constructor: initializes Cloudflare API wrapper with authentication and Curl
Labs_Core::Cloudflare::Cloudflare(const API_Auth& auth)
	: auth(auth), curl(Curl_Wrapper()), must_cout(true)
{
}

// Overloaded constructor allows toggling console output
Labs_Core::Cloudflare::Cloudflare(const API_Auth& auth, bool must_cout)
	: auth(auth), curl(Curl_Wrapper()), must_cout(must_cout)
{
}

// -----------------------------------------------------------------------------
// API Response Helpers
// -----------------------------------------------------------------------------

// Parses Cloudflare API JSON response to return success or error info.
// Returns a vector of tuples with (error_code, error_message) or success status.
std::vector<std::tuple<int, std::string>> Labs_Core::Cloudflare::Get_Return_Info(json responce) {
	if (responce["success"]) {
		return { std::tuple<int, std::string>(0, "Success") };
	}
	else {
		json errors = responce["errors"];
		std::vector<std::tuple<int, std::string>> errors_map = {};
		for (json error : errors) {
			errors_map.push_back(std::tuple<int, std::string>(error["code"], error["message"]));
		}
		return errors_map;
	}
}

// -----------------------------------------------------------------------------
// API Testing Utilities
// -----------------------------------------------------------------------------

// Tests the validity of the stored API token by calling Cloudflare token verify endpoint.
// Returns status string or error message.
std::string Labs_Core::Cloudflare::Test_API()
{
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/tokens/verify";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce;
	try {
		responce = std::get<std::string>(curl.Get(url, headers));
	}
	catch (const char*) {
		return "Failed to fetch";
	}

	try {
		json JSON = json::parse(responce);
		if (JSON["success"]) {
			return JSON["result"]["status"];
		}
		return "Invalid Authorization";
	}
	catch (const char*) {
		return "Failed to parse";
	}
}

// Static helper to test API given an API_Auth object.
std::string Labs_Core::Cloudflare::Test_API(const API_Auth& auth) {
	return Labs_Core::Cloudflare(auth).Test_API();
}
