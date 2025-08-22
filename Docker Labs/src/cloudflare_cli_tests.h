#include "cloudflare_hook.h"

namespace Docker_Labs::Cloudflare::Commands {
	int Test_API(API_Auth cf_auth);
	int Test_Ingress(API_Auth cf_auth);
	int Test_DNS(API_Auth cf_auth);
	int Test_Application(API_Auth cf_auth);
	int Test_Initialize(API_Auth cf_auth);
	int Test_Grant_Policy(API_Auth cf_auth);
}