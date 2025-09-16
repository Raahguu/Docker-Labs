#pragma once
#include <string>
#include <vector>

namespace Docker_Labs::Labs_Core {
	class Container;
}

namespace Docker_Labs::Labs_Core {

        class Network {
        public:
                explicit Network(std::string id);

                //Accessors
                std::string Get_ID();
                //Cache Accessors
                std::string Get_Name_Cache();
                std::string Get_Subnet_Cache();
                std::string Get_Gateway_Cache();
                std::vector<Labs_Core::Container> Get_Containers_Cache();
		std::string Get_IP_Range_Cache();
                int Cache_Update();
        private:
                std::string id;
                std::string name_cache;
                std::string subnet_cache;
                std::string gateway_cache;
                std::vector<Labs_Core::Container> containers_cache;
		std::string IPRange;

        };

}
