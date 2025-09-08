#pragma once
#include <string>
#include <vector>
#include "labs_container.h"

namespace Docker_Labs {

        class Network {
        public:
                explicit Network(std::string id);

                //Accessors
                std::string Get_ID();
                //Cache Accessors
                std::string Get_Name_Cache();
                std::string Get_Subnet_Cache();
                std::string Get_Gateway_Cache();
                std::vector<Docker_Labs::Container> Get_Containers_Cache();
                int Cache_Update();
        private:
                std::string id;
                std::string name_cache;
                std::string subnet_cache;
                std::string gateway_cache;
                std::vector<Docker_Labs::Container> containers_cache;

        };

}
