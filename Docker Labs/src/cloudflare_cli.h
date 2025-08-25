#include <string>
#include <tuple>
#include "main.h"
#include "cloudflare_hook.h"

namespace Docker_Labs::Cloudflare::Commands {
	int Command_Handler(Command_Interpreter command, int argc, char* argv[]);
	int Get_Seats(API_Auth cf_auth);
	int Fetch_Ingress(API_Auth cf_auth);
	int Fetch_DNS_Records(API_Auth cf_auth);


	Container Spec_Container(int argc, char* argv[]);
	std::tuple<Container,User> Spec_Container_User(int argc, char* argv[]);

	int Create_Ingress(API_Auth cf_auth, int argc, char* argv[]);
	int Update_Ingress(API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Ingress(API_Auth cf_auth, int argc, char* argv[]);

	int Create_DNS(API_Auth cf_auth, int argc, char* argv[]);
	int Remove_DNS(API_Auth cf_auth, int argc, char* argv[]);

	int Create_Application(API_Auth cf_auth, int argc, char* argv[]);
	int Remove_Application(API_Auth cf_auth, int argc, char* argv[]);

	int Grant_Container(API_Auth cf_auth, int argc, char* argv[]);
	int Revoke_Container(API_Auth cf_auth, int argc, char* argv[]);
}