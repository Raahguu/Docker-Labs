#pragma once
#include "cloudflare_hook.h"

namespace Docker_Labs::Labs_CLI::Cloudflare {
	int Test_API(Docker_Labs::Cloudflare::API_Auth cf_auth);
	int Test_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth);
	int Test_DNS(Docker_Labs::Cloudflare::API_Auth cf_auth);
	int Test_Application(Docker_Labs::Cloudflare::API_Auth cf_auth);
	int Test_Initialize(Docker_Labs::Cloudflare::API_Auth cf_auth);
	int Test_Grant_Policy(Docker_Labs::Cloudflare::API_Auth cf_auth);
}