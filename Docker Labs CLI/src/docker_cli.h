#include "main.h"
#include "labs_container.h"

namespace Docker_Labs::Labs_CLI::Docker{
		//Handles figuring out which function each command should go to
		int Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Test_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Get_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		
		//Actual commands
		int Help(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Test_API(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Test_Container_Control(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Start(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Stop(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Restart(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Reset(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		
		//Gets
		int Get_Container_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		int Get_Network_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
}
