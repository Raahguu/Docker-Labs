/**
 * cloudflare_cli_tests.h
 * Test declarations for the Cloudflare CLI module.
 *
 * Provides test function declarations to verify the correctness
 * of Cloudflare CLI command implementations.
 */


#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"

namespace Docker_Labs::Labs_CLI::Cloudflare {

	// === TEST FUNCTIONS ===
	// These functions are used for validating Cloudflare integration
	// through various API endpoints and features.

	// Verifies API authentication and connection to Cloudflare.
	int Test_API(Labs_Core::Cloudflare::API_Auth cf_auth);

	// Tests ingress creation, retrieval, and cleanup operations.
	int Test_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth);

	// Tests DNS record creation and deletion.
	int Test_DNS(Labs_Core::Cloudflare::API_Auth cf_auth);

	// Tests application creation/removal (e.g., Access apps).
	int Test_Application(Labs_Core::Cloudflare::API_Auth cf_auth);

	// Tests a full initialization sequence or bootstrap setup.
	int Test_Initialize(Labs_Core::Cloudflare::API_Auth cf_auth);

	// Tests granting policy (e.g., container access control rules).
	int Test_Grant_Policy(Labs_Core::Cloudflare::API_Auth cf_auth);
}
