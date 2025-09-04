#include "main.h"

namespace Docker_Labs {
	int Rm_Handler(int argc, char* argv[]);
	int Nuke(int argc, char* argv[]);
	int Rm(std::string container_name, bool keep_container);
}
