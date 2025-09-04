#include <unistd.h>
#include "cloudflare_cli_tests.h"
#include "cloudflare_cli.h"

int Docker_Labs::Labs_CLI::Cloudflare::Test_API(Docker_Labs::Cloudflare::API_Auth cf_auth) {
    int responce_code = Docker_Labs::Cloudflare::Test_API(cf_auth);
    switch (responce_code)
    {
    case 0:
        std::cout << "Pass" << std::endl;
        return 0;
    case 2:
        std::cout << "Authorization Failure" << std::endl;
        return 1;
    case 3:
        std::cout << "Connection Failure" << std::endl;
        return 1;
    default:
        std::cout << "Unknown Failure" << std::endl;
        return 1;
    }
}

int Docker_Labs::Labs_CLI::Cloudflare::Test_Ingress(Docker_Labs::Cloudflare::API_Auth cf_auth)
{
    Docker_Labs::Cloudflare::Cloudflared cloudflared = Docker_Labs::Cloudflare::Cloudflared(cf_auth);
    std::cout << "Creating bogus ingress rule." << std::endl;
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    cloudflared.Create_Ingress(container);
    std::cout << "Created." << std::endl << "Waiting 10 seconds before update..." << std::endl;
    sleep(10);
    container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "172.17.0.2", { "a", "b" });
    cloudflared.Update_Ingress(container);
    std::cout << "Updated." << std::endl << "Waiting 10 seconds before removal..." << std::endl;
    sleep(10);
    cloudflared.Remove_Ingress(container);
    std::cout << "Removed." << std::endl;

    return 0;
}
int Docker_Labs::Labs_CLI::Cloudflare::Test_DNS(Docker_Labs::Cloudflare::API_Auth cf_auth)
{
    Docker_Labs::Cloudflare::Cloudflared cloudflared = Docker_Labs::Cloudflare::Cloudflared(cf_auth);
    std::cout << "Creating bogus DNS record..." << std::endl;
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    cloudflared.Create_DNS_Record(container);
    std::cout << "Waiting 1 second for lookup..." << std::endl;
    sleep(1);
    Docker_Labs::Labs_CLI::Cloudflare::Fetch_DNS_Records(cf_auth);
    std::cout << "Confirm the bogus record is in the list above." << std::endl << std::endl;
    std::cout << "Waiting 9 seconds before removal..." << std::endl;
    sleep(9);
    cloudflared.Remove_DNS_Record(container);
    std::cout << "Removed." << std::endl;

    return 0;
}
int Docker_Labs::Labs_CLI::Cloudflare::Test_Application(Docker_Labs::Cloudflare::API_Auth cf_auth)
{
    Docker_Labs::Cloudflare::Cloudflared cloudflared = Docker_Labs::Cloudflare::Cloudflared(cf_auth);
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    std::cout << "Creating bogus Access Application..." << std::endl;
    cloudflared.Create_Application(container);
    std::cout << "Waiting 10 second before removal..." << std::endl;
    sleep(10);
    cloudflared.Remove_Application(container);
    std::cout << "Removed." << std::endl;

    return 0;
}


int Docker_Labs::Labs_CLI::Cloudflare::Test_Initialize(Docker_Labs::Cloudflare::API_Auth cf_auth)
{
    Docker_Labs::Cloudflare::Cloudflared cloudflared = Docker_Labs::Cloudflare::Cloudflared(cf_auth);
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    cloudflared.Create_Ingress(container);
    cloudflared.Create_DNS_Record(container);
    cloudflared.Create_Application(container);
    std::cout << "Full bogus exposed in cloudflare." << std::endl;
    std::cout << "Waiting 10 seconds before removal..." << std::endl;
    sleep(10);
    cloudflared.Remove_Application(container);
    cloudflared.Remove_DNS_Record(container);
    cloudflared.Remove_Ingress(container);
    std::cout << "Removed." << std::endl;
    return 0;
}

int Docker_Labs::Labs_CLI::Cloudflare::Test_Grant_Policy(Docker_Labs::Cloudflare::API_Auth cf_auth) {
    Docker_Labs::Cloudflare::Cloudflared cloudflared = Docker_Labs::Cloudflare::Cloudflared(cf_auth);
    Container container = Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    User add = User("joshua.finlayson@cassa.au");
    User remove = User("laith.striegher@cassa.au");
    cloudflared.Create_Ingress(container);
    cloudflared.Create_DNS_Record(container);
    cloudflared.Create_Application(container);
    cloudflared.Grant_Container(container, add);
    cloudflared.Revoke_Container(container, remove);
    std::cout << "Full bogus exposed in cloudflare." << std::endl;
    std::cout << "Waiting for 90 seconds before removal..." << std::endl;
    sleep(90);
    cloudflared.Remove_Application(container);
    cloudflared.Remove_DNS_Record(container);
    cloudflared.Remove_Ingress(container);
    std::cout << "Removed." << std::endl;
    return 0;
}