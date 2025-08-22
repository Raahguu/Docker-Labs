#include "main.h"
#include "labs_container.h"

namespace Docker_Labs::Docker::Commands{
		//Handles figureing out which function each command should go to
		int Command_Handler(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		
		//Actual commands
		int Help(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		int Test_API(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		int Test_Container_Creation(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		int Test_Handler(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
}
