// Docker Labs.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
using namespace std;

namespace Docker_Labs {
	struct User {
		std::string email;
		std::string cf_uid;
	};
	struct Container {
		std::string docker_id;
		std::string name;
		std::string image;
		std::string ip_addr;
		std::string net_id;
	};
}

// TODO: Reference additional headers your program requires here.
