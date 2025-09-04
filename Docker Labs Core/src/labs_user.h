#pragma once
#include <string>

namespace Docker_Labs {
	class User {
	public:
		User(std::string email);
		std::string Get_Email();
	private:
		const std::string email;
	};
}