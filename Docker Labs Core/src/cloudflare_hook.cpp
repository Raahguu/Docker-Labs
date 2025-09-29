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
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

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

// Returns the lexicographically first UUID string from /dev/disk/by-uuid
// Used as a key for XOR obfuscation
std::string getFirstUUID() {
	std::string first;
	bool found = false;
	for (const auto& entry : std::filesystem::directory_iterator("/dev/disk/by-uuid")) {
		std::string name = entry.path().filename().string();
		if (!found || name < first) {
			first = name;
			found = true;
		}
	}
	return found ? first : "";
}

// Applies XOR cipher to input string using the provided key
// Used to obfuscate and de-obfuscate the API credentials string
std::string xorCipher(const std::string& input, const std::string& key) {
	std::string output = input;
	for (size_t i = 0; i < input.size(); ++i) {
		output[i] = input[i] ^ key[i % key.size()];
	}
	return output;
}

// -----------------------------------------------------------------------------
// API_Auth: Connection String Encoding/Decoding
// -----------------------------------------------------------------------------

// Generates a Base64-encoded and XOR-obfuscated connection string 
// that contains concatenated API credentials.
// Uses system UUID as the XOR key.
std::string Labs_Core::Cloudflare::API_Auth::Generate_Connection_String() {
	using It = boost::archive::iterators::base64_from_binary<
		boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8>
	>;

	std::string UUID = getFirstUUID();
	if (UUID.empty()) {
		throw std::runtime_error("NO UUID found for connection string generation");
	}

	// Concatenate all credential fields into a single plaintext string
	std::string plaintext = ACC + ZONE + TUNN + TKN + DOMN;

	// XOR obfuscate using UUID as key
	std::string ciphertext = xorCipher(plaintext, UUID);

	// Base64 encode the obfuscated string
	auto tmp = std::string(It(std::begin(ciphertext)), It(std::end(ciphertext)));

	// Add necessary padding characters to make valid Base64
	return tmp.append((3 - ciphertext.size() % 3) % 3, '=');
}

// Decodes and de-obfuscates a Base64 connection string into an API_Auth object.
// Assumes XOR key is the system UUID.
Labs_Core::Cloudflare::API_Auth Labs_Core::Cloudflare::API_Auth::From_Connection_String(const std::string& connection_string) {
	using It = boost::archive::iterators::transform_width<
		boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6
	>;

	std::string UUID = getFirstUUID();
	if (UUID.empty()) {
		throw std::runtime_error("NO UUID found for connection string decoding");
	}

	// Decode the Base64 connection string
	std::string ciphertext = boost::algorithm::trim_right_copy_if(std::string(It(std::begin(connection_string)), It(std::end(connection_string))), [](char c) {
		return c == '\0';
		});

	// XOR de-obfuscate to retrieve plaintext credentials
	std::string plaintext = xorCipher(ciphertext, UUID);

	// Validate minimum length (sum of all field lengths)
	if (plaintext.size() < 140) { // 32 + 32 + 36 + 40 = 140 (DOMN length may vary)
		throw std::runtime_error("Invalid connection string format");
	}

	// Extract each credential by substring offsets
	std::string ACC = plaintext.substr(0, 32);
	std::string ZONE = plaintext.substr(32, 32);
	std::string TUNN = plaintext.substr(64, 36);
	std::string TKN = plaintext.substr(100, 40);
	std::string DOMN = plaintext.substr(140);

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
