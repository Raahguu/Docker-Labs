#include <unistd.h>
#include "cloudflare_cli_tests.h"
#include "cloudflare_cli.h"

using namespace Docker_Labs;

int Labs_CLI::Cloudflare::Test_API(Labs_Core::Cloudflare::API_Auth cf_auth) {
    int responce_code = Labs_Core::Cloudflare::Test_API(cf_auth);
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

int Labs_CLI::Cloudflare::Test_Ingress(Labs_Core::Cloudflare::API_Auth cf_auth)
{
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);
    std::cout << "Creating bogus ingress rule." << std::endl;
    Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    cloudflare.Create_Ingress(container);
    std::cout << "Created." << std::endl << "Waiting 10 seconds before update..." << std::endl;
    sleep(10);
    container = Labs_Core::Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "172.17.0.2", { "a", "b" });
    cloudflare.Update_Ingress(container);
    std::cout << "Updated." << std::endl << "Waiting 10 seconds before removal..." << std::endl;
    sleep(10);
    cloudflare.Remove_Ingress(container);
    std::cout << "Removed." << std::endl;

    return 0;
}
int Labs_CLI::Cloudflare::Test_DNS(Labs_Core::Cloudflare::API_Auth cf_auth)
{
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);
    std::cout << "Creating bogus DNS record..." << std::endl;
    Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    cloudflare.Create_DNS_Record(container);
    std::cout << "Waiting 1 second for lookup..." << std::endl;
    sleep(1);
    Labs_CLI::Cloudflare::Fetch_DNS_Records(cf_auth);
    std::cout << "Confirm the bogus record is in the list above." << std::endl << std::endl;
    std::cout << "Waiting 9 seconds before removal..." << std::endl;
    sleep(9);
    cloudflare.Remove_DNS_Record(container);
    std::cout << "Removed." << std::endl;

    return 0;
}
int Labs_CLI::Cloudflare::Test_Application(Labs_Core::Cloudflare::API_Auth cf_auth)
{
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);
    Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    std::cout << "Creating bogus Access Application..." << std::endl;
    cloudflare.Create_Application(container);
    std::cout << "Waiting 10 second before removal..." << std::endl;
    sleep(10);
    cloudflare.Remove_Application(container);
    std::cout << "Removed." << std::endl;

    return 0;
}


int Labs_CLI::Cloudflare::Test_Initialize(Labs_Core::Cloudflare::API_Auth cf_auth)
{
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);
    Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    cloudflare.Create_Ingress(container);
    cloudflare.Create_DNS_Record(container);
    cloudflare.Create_Application(container);
    std::cout << "Full bogus exposed in cloudflare." << std::endl;
    std::cout << "Waiting 10 seconds before removal..." << std::endl;
    sleep(10);
    cloudflare.Remove_Application(container);
    cloudflare.Remove_DNS_Record(container);
    cloudflare.Remove_Ingress(container);
    std::cout << "Removed." << std::endl;
    return 0;
}

int Labs_CLI::Cloudflare::Test_Grant_Policy(Labs_Core::Cloudflare::API_Auth cf_auth) {
    Labs_Core::Cloudflare cloudflare = Labs_Core::Cloudflare(cf_auth);
    Labs_Core::Container container = Labs_Core::Container::Bogus("bogus", "laith_striegher_cassa_au_b0g", "image", "127.0.0.1", { "a", "b" });
    Labs_Core::User add = Labs_Core::User("joshua.finlayson@cassa.au");
    Labs_Core::User remove = Labs_Core::User("laith.striegher@cassa.au");
    cloudflare.Create_Ingress(container);
    cloudflare.Create_DNS_Record(container);
    cloudflare.Create_Application(container);
    cloudflare.Grant_Container(container, add);
    cloudflare.Revoke_Container(container, remove);
    std::cout << "Full bogus exposed in cloudflare." << std::endl;
    std::cout << "Waiting for 90 seconds before removal..." << std::endl;
    sleep(90);
    cloudflare.Remove_Application(container);
    cloudflare.Remove_DNS_Record(container);
    cloudflare.Remove_Ingress(container);
    std::cout << "Removed." << std::endl;
    return 0;
}