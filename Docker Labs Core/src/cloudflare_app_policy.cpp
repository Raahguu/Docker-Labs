// -----------------------------------------------------------------------------
// @file cloudflare_app_policy.cpp
// @brief Implements Cloudflare Zero Trust application and access policy logic.
//
// This file defines methods for managing Cloudflare Access Applications and 
// their associated access policies. It includes functionality for creating, 
// fetching, and deleting apps, as well as granting/revoking user access.
//
// Dependencies:
// - docker_labs/core/cloudflare_hook.h
// - docker_labs/core/docker_hook.h
// - docker_labs/core/labs_user.h
// - docker_labs/core/curl_wrapper.h
// -----------------------------------------------------------------------------

#include <tuple>
#include <iostream>
#include <algorithm>  // for std::remove_if
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"
#include "docker_labs/core/curl_wrapper.h"
#include "docker_labs/core/labs_user.h"

using namespace Docker_Labs;

// -----------------------------------------------------------------------------
// Application Management
// -----------------------------------------------------------------------------

// Fetches the Cloudflare Access application details for a given container.
// Uses the container's cached name and the domain from authentication.
json Labs_Core::Cloudflare::Fetch_Application(Labs_Core::Container container) {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps?exact=true&domain=" + container.Get_Name_Cache() + "-" + auth.DOMN;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));

	// Parse the JSON response from Cloudflare API
	json application = json::parse(responce);
	return application;
}

// Generates the JSON payload to create a new Cloudflare Access application.
// The application type is SSH with specific session duration and redirect behavior.
std::string Labs_Core::Cloudflare::Generate_Add_Application_Config(Container container, std::string name) {
	json message_body = "{\"type\":\"ssh\", \"session_duration\":\"12h\",\"auto_redirect_to_identity\":false,\"allow_iframe\":true}"_json;
	message_body["domain"] = container.Get_Name_Cache() + "-" + auth.DOMN;
	message_body["name"] = name;
	return message_body.dump();
}

// Convenience overload: creates an application using the container's cached name.
int Labs_Core::Cloudflare::Create_Application(Container container) {
	return Create_Application(container, container.Get_Name_Cache());
}

// Sends a POST request to Cloudflare to create a new application.
// The payload is generated with the specified name.
int Labs_Core::Cloudflare::Create_Application(Container container, std::string name) {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};

	std::string data = Generate_Add_Application_Config(container, name);
	std::string responce = std::get<std::string>(curl.Post(url, data, headers));
	// Parse the response (not used here but could be for error checking)
	json parsed_responce = json::parse(responce);
	if (parsed_responce["success"]) {
		if (must_cout) std::cout << "Application created successfully." << std::endl;
		return 0;
	}
	else {
		std::cout << "Failed to create application. Errors:" << std::endl;
		for (const auto& error : parsed_responce["errors"]) {
			std::cout << "Code: " << error["code"] << ", Message: " << error["message"] << std::endl;
		}
		return parsed_responce["errors"][0]["code"];
	}
}

// Removes the Cloudflare Access application associated with the container.
// Fetches the application first to obtain its ID, then sends a DELETE request.
int Labs_Core::Cloudflare::Remove_Application(Container container) {
	json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id;
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Delete(url, headers));
	if (must_cout) {
		std::cout << "Removed application for '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
	}
	return 0;
}

// -----------------------------------------------------------------------------
// Access Policy Management
// -----------------------------------------------------------------------------

// Initializes an access policy for a user on a container's application.
// Fetches the application ID automatically.
int Labs_Core::Cloudflare::Initialize_Policy(Container container, User user) {
	json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	return Initialize_Policy(container, user, application_id);
}

// Sends a POST request to create a new access policy for the user on the application.
int Labs_Core::Cloudflare::Initialize_Policy(Container container, User user, std::string application_id) {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies";
	std::vector<std::string> headers = {
		"Content-Type: application/json",
		"Authorization: Bearer " + auth.TKN
	};
	std::string data = Generate_Initial_Policy_Config(container, user);
	std::string responce = std::get<std::string>(curl.Post(url, data, headers));
	if (must_cout) {
		std::cout << "Created access policy for '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Granted access to '" + user.Get_Email() + "' for container '" + container.Get_Name_Cache() + "'" << std::endl;
		std::cout << "Application ID: '" + application_id + "'" << std::endl;
	}
	return 0;
}

// Creates the JSON payload for a new access policy that grants a specific user access.
std::string Labs_Core::Cloudflare::Generate_Initial_Policy_Config(Container container, User user) {
	json message_body = "{\"decision\":\"allow\",\"include\":[{\"email\":{}}]}"_json;
	message_body["name"] = "Access Policy for '" + container.Get_Name_Cache() + "'";
	message_body["include"][0]["email"]["email"] = user.Get_Email();
	return message_body.dump();
}

// Generates an updated policy JSON that adds the user to the list of allowed emails.
std::string Labs_Core::Cloudflare::Generate_Grant_Policy_Config(Labs_Core::Container container, Labs_Core::User user, json application_policy) {
	json result = application_policy["result"];

	// If no existing policies, create initial one
	if (result.empty()) {
		return Generate_Initial_Policy_Config(container, user);
	}

	json message_body = "{\"decision\":\"allow\"}"_json;
	message_body["name"] = result[0]["name"];
	json granted = result[0]["include"];
	json target = "{\"email\":{}}"_json;
	target["email"]["email"] = user.Get_Email();
	granted.push_back(target);
	message_body["include"] = granted;
	return message_body.dump();
}

// Generates an updated policy JSON that removes the user from the list of allowed emails.
std::string Labs_Core::Cloudflare::Generate_Revoke_Policy_Config(Labs_Core::Container container, Labs_Core::User user, json application_policy) {
	json message_body = "{\"decision\":\"allow\"}"_json;
	message_body["name"] = application_policy["result"][0]["name"];
	json granted = application_policy["result"][0]["include"];

	// Remove the user with matching email from the allowed list
	granted.erase(
		std::remove_if(granted.begin(), granted.end(), [&](json& obj) {
			return obj["email"].contains("email") && obj["email"]["email"] == user.Get_Email();
			}),
		granted.end()
	);

	message_body["include"] = granted;
	return message_body.dump();
}

// Fetches all access policies for the application's container.
json Labs_Core::Cloudflare::Fetch_Application_Policy(Labs_Core::Container container) {
	json application = Fetch_Application(container);
	std::string application_id = application["result"][0]["id"];
	return Fetch_Application_Policy(container, application_id);
}

// Fetches access policies for a specific application ID.
json Labs_Core::Cloudflare::Fetch_Application_Policy(Labs_Core::Container container, std::string application_id) {
	std::string url = "https://api.cloudflare.com/client/v4/accounts/" + auth.ACC + "/access/apps/" + application_id + "/policies";
	std::vector<std::string> headers = {
		"Authorization: Bearer " + auth.TKN
	};
	std::string responce = std::get<std::string>(curl.Get(url, headers));
	json policies = json::parse(responce);
	return policies;
}
