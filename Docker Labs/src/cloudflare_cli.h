#include <string>
#include "main.h"
#include "cloudflare_hook.h"

namespace Docker_Labs::Cloudflare::Commands {
	int Command_Handler(Command_Interpreter command, int argc, char* argv[]);
	int Test_API(API_Auth cf_auth);
	int Get_Seats(API_Auth cf_auth);
	int Fetch_Ingress(API_Auth cf_auth);
	int Test_Ingress(API_Auth cf_auth);
	int Fetch_DNS_Records(API_Auth cf_auth);
	int Test_DNS(API_Auth cf_auth);
	int Test_Application(API_Auth cf_auth);
	int Test_Initial_Policy(API_Auth cf_auth);
}