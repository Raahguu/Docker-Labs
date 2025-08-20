#pragma once
#include <string>

namespace Docker_Labs {
	class User {
	public:
		User(std::string email, std::string cf_uid);
		std::string Get_Email();
		std::string Get_CF_UID();
	private:
		const std::string email;
		const std::string cf_uid;
	};
}