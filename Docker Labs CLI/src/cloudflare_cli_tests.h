#pragma once
#include "main.h"
#include "cloudflare_hook.h"

namespace Docker_Labs::Labs_CLI::Cloudflare {
	int Test_API(Labs_Core::Cloudflare::API_Auth cf_auth);
	int Test_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth);
	int Test_DNS(Labs_Core::Cloudflare::API_Auth cf_auth);
	int Test_Application(Labs_Core::Cloudflare::API_Auth cf_auth);
	int Test_Initialize(Labs_Core::Cloudflare::API_Auth cf_auth);
	int Test_Grant_Policy(Labs_Core::Cloudflare::API_Auth cf_auth);
}