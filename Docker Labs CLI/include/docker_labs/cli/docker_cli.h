/**
 * docker_cli.h
 * Declarations for Docker CLI commands.
 *
 * Defines functions and classes that manage Docker container-related
 * commands in the CLI application.
 */


#include "main.h"
#include "docker_labs/core/labs_container.h"

namespace Docker_Labs::Labs_CLI::Docker {

	// === DISPATCHERS / ROUTERS ===
	// Route CLI input to the appropriate category of command.
	int Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Test_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Get_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);


	// === CORE CONTROL COMMANDS ===
	// These manage Docker container lifecycle operations.
	int Start(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Stop(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Restart(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Reset(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);


	// === INFORMATIONAL / QUERY COMMANDS ===
	// Used to retrieve runtime and configuration details.
	int Get_Container_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Get_Network_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);


	// === TEST COMMANDS ===
	// Functions used for testing Docker integration and control.
	int Test_API(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Test_Container_Control(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);


	// === MISCELLANEOUS ===
	// Help and CLI-related utilities.
	int Help(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
}
