#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"

namespace Docker_Labs::Labs_CLI::Cloudflare {

	/// <summary>
	/// Verifies API authentication and connection to Cloudflare.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API authentication credentials.</param>
	/// <returns>Status code.</returns>
	int Test_API(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Tests ingress creation, retrieval, and cleanup operations.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API authentication credentials.</param>
	/// <returns>Status code.</returns>
	int Test_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Tests DNS record creation and deletion.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API authentication credentials.</param>
	/// <returns>Status code.</returns>
	int Test_DNS(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Tests application creation and removal (e.g., Access apps).
	/// </summary>
	/// <param name="cf_auth">Cloudflare API authentication credentials.</param>
	/// <returns>Status code.</returns>
	int Test_Application(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Tests a full initialization sequence or bootstrap setup.
	/// </summary>
	/// <param name="cf_auth">Cloudflare API authentication credentials.</param>
	/// <returns>Status code.</returns>
	int Test_Initialize(Labs_Core::Cloudflare::API_Auth cf_auth);

	/// <summary>
	/// Tests granting policy (e.g., container access control rules).
	/// </summary>
	/// <param name="cf_auth">Cloudflare API authentication credentials.</param>
	/// <returns>Status code.</returns>
	int Test_Grant_Policy(Labs_Core::Cloudflare::API_Auth cf_auth);
}
