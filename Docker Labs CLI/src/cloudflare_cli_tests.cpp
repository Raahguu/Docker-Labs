/**
 * cloudflare_cli_tests.cpp
 * Implementation of tests for the Cloudflare CLI module.
 *
 * Contains test cases to validate the Cloudflare CLI command behavior
 * and verify proper API integration.
 */

#include <thread>
#include <chrono>
#include "docker_labs/cli/cloudflare_cli_tests.h"
#include "docker_labs/cli/cloudflare_cli.h"

using namespace Docker_Labs;

// Simple API test to check Cloudflare connectivity and response
int Labs_CLI::Cloudflare::Test_API(Labs_Core::Cloudflare::API_Auth cf_auth) {
	std::string responce = Labs_Core::Cloudflare::Test_API(cf_auth);
	std::cout << responce << std::endl;
	return 0;
}

// Tests creating, updating, and removing an ingress rule with delays in between
int Labs_CLI::Cloudflare::Test_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth)
{
	Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);

	std::cout << "Creating bogus ingress rule." << std::endl;
	// Create a dummy container with localhost IP for testing ingress
	Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "bogus_email_email_org_b0g", "image", "127.0.0.1");
	cloudflare.Create_Ingress(container);
	std::cout << "Created." << std::endl << "Waiting 10 seconds before update..." << std::endl;

	// Wait for 10 seconds simulating delay before update
	std::this_thread::sleep_for(std::chrono::seconds(10));

	// Update the container with a different IP and update ingress rule accordingly
	container = Labs_Core::Container::Bogus("bogus", "bogus_email_email_org_b0g", "image", "172.17.0.2");
	cloudflare.Update_Ingress(container);
	std::cout << "Updated." << std::endl << "Waiting 10 seconds before removal..." << std::endl;

	// Wait again before removing the ingress rule
	std::this_thread::sleep_for(std::chrono::seconds(10));

	cloudflare.Remove_Ingress(container);
	std::cout << "Removed." << std::endl;

	return 0;
}

// Tests creating and removing a DNS record with a short delay to verify creation
int Labs_CLI::Cloudflare::Test_DNS(Labs_Core::Cloudflare::API_Auth cf_auth)
{
	Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);
	std::cout << "Creating bogus DNS record..." << std::endl;

	// Create a dummy container and DNS record
	Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "bogus_email_email_org_b0g", "image", "127.0.0.1");
	cloudflare.Create_DNS_Record(container);

	std::cout << "Waiting 1 second for lookup..." << std::endl;
	// Wait briefly to allow DNS record propagation for fetch test
	std::this_thread::sleep_for(std::chrono::seconds(1));

	// Fetch DNS records to verify the bogus record is present
	Labs_CLI::Cloudflare::Fetch_DNS_Records(cf_auth);

	std::cout << "Confirm the bogus record is in the list above." << std::endl << std::endl;
	std::cout << "Waiting 10 seconds before removal..." << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(10));

	// Remove the bogus DNS record
	cloudflare.Remove_DNS_Record(container);
	std::cout << "Removed." << std::endl;

	return 0;
}

// Tests creating and removing a Cloudflare Access Application for a dummy container
int Labs_CLI::Cloudflare::Test_Application(Labs_Core::Cloudflare::API_Auth cf_auth)
{
	Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);

	Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "bogus_email_email_org_b0g", "image", "127.0.0.1");

	std::cout << "Creating bogus Access Application..." << std::endl;
	cloudflare.Create_Application(container);

	std::cout << "Waiting 10 second before removal..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));

	cloudflare.Remove_Application(container);
	std::cout << "Removed." << std::endl;

	return 0;
}

// Comprehensive test initializing ingress, DNS, and application for a bogus container then cleaning up
int Labs_CLI::Cloudflare::Test_Initialize(Labs_Core::Cloudflare::API_Auth cf_auth)
{
	Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);

	Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "bogus_email_email_org_b0g", "image", "127.0.0.1");

	// Create ingress, DNS record, and application all at once
	cloudflare.Create_Ingress(container);
	cloudflare.Create_DNS_Record(container);
	cloudflare.Create_Application(container);

	std::cout << "Full bogus exposed in cloudflare." << std::endl;
	std::cout << "Waiting 10 seconds before removal..." << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(10));

	// Cleanup all created resources
	cloudflare.Remove_Application(container);
	cloudflare.Remove_DNS_Record(container);
	cloudflare.Remove_Ingress(container);
	std::cout << "Removed." << std::endl;

	return 0;
}

// Tests granting and revoking container access permissions for specific users
int Labs_CLI::Cloudflare::Test_Grant_Policy(Labs_Core::Cloudflare::API_Auth cf_auth) {
	Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);

	Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "bogus_email_email_org_b0g", "image", "127.0.0.1");

	// Define users to add and remove access for
	Labs_Core::User add = Labs_Core::User("bogus.other@email.org");
	Labs_Core::User remove = Labs_Core::User("bogus.email@email.org");

	// Create the necessary Cloudflare resources
	cloudflare.Create_Ingress(container);
	cloudflare.Create_DNS_Record(container);
	cloudflare.Create_Application(container);

	// Grant access to one user and revoke from another
	cloudflare.Grant_Container(container, add);
	cloudflare.Revoke_Container(container, remove);

	std::cout << "Full bogus exposed in cloudflare." << std::endl;
	std::cout << "Waiting for 90 seconds before removal..." << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(10));

	// Cleanup resources after testing
	cloudflare.Remove_Application(container);
	cloudflare.Remove_DNS_Record(container);
	cloudflare.Remove_Ingress(container);
	std::cout << "Removed." << std::endl;

	return 0;
}
