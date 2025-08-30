#include "main.h"
#include "labs_container.h"

namespace Docker_Labs::Docker::Commands{
		//Handles figuring out which function each command should go to
		int Command_Handler(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		int Test_Handler(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		
		//Actual commands
		int Help(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		int Test_API(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
		int Test_Container_Control(Docker_Labs::Command_Interpreter command, int argc, char* argv[]);
}
