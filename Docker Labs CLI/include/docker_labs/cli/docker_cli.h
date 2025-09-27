#include "main.h"
#include "docker_labs/core/labs_container.h"

namespace Docker_Labs::Labs_CLI::Docker {

	/// <summary>
	/// DISPATCHERS / ROUTERS
	/// <para>Route CLI input to the appropriate category of command.</para>
	/// </summary>

	/// <summary>
	/// Routes CLI input to the appropriate Docker command handler.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Command_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Routes CLI input to the appropriate Docker test handler.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Test_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Routes CLI input to the appropriate Docker get/query handler.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Get_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// CORE CONTROL COMMANDS
	/// <para>Manage Docker container lifecycle operations.</para>
	/// </summary>

	/// <summary>
	/// Starts a Docker container.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Start(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Stops a Docker container.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Stop(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Restarts a Docker container.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Restart(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Resets a Docker container to its initial state.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Reset(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// INFORMATIONAL / QUERY COMMANDS
	/// <para>Retrieve runtime and configuration details.</para>
	/// </summary>

	/// <summary>
	/// Retrieves information about a Docker container.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Get_Container_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Retrieves network information for Docker containers.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Get_Network_Info(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// TEST COMMANDS
	/// <para>Functions used for testing Docker integration and control.</para>
	/// </summary>

	/// <summary>
	/// Tests Docker API integration.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Test_API(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// Tests Docker container control operations.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Test_Container_Control(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// MISCELLANEOUS
	/// <para>Help and CLI-related utilities.</para>
	/// </summary>

	/// <summary>
	/// Displays help or usage information for Docker CLI commands.
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Help(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);
}
