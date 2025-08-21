#pragma once
#include <string>
#include <vector>

namespace Docker_Labs {

	class Container {
		public:
			explicit Container(std::string id);
			Container(std::string email, std::string image_name, std::string container_name);
			Container(std::string email, std::string image_name);
			Container(User user, std::string image_name, std::string container_name);
			Container(User user, std::string image_name);

			//Accessors
			std::string Get_ID();
			std::string Get_Name();
			std::string Get_Image();
			std::string Get_IP();
			std::vector<std::string> Get_Networks();
			std::string Get_Owner();
			bool Get_Status();

			//Controls
			int Start();
			int Stop();
			int Restart();
			int Kill();
			int Remove();
			int Set_Owner(std::string email);
			int Set_Owner(User user);

		private:
			std::string id;
	};

	namespace Docker {
		Container Get_Container(std::string container_name);
		std::vector<Container> Get_Owned_Containers(std::string email);
		std::vector<Container> Get_Owned_Containers(User user);
		int Start(User user);
		int Stop(User user);
		int Restart(User user);
		int Kill(User user);
		int Remove(User user);
	}

}
