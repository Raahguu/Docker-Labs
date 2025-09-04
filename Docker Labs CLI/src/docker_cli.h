#pragma once
#include "main.h"

namespace Docker_Labs::Labs_CLI::Docker{
	//Handles figuring out which function each command should go to
	int Command_Handler(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Test_Handler(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		
	//Actual commands
	int Help(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Test_API(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Test_Container_Control(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
		
	int Start(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Stop(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Restart(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
	int Reset(Docker_Labs::Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
}
