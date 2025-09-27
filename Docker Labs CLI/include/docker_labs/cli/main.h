#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace Docker_Labs::Labs_CLI {

	/// <summary>
	/// Main command interpreter for the Docker Labs CLI.
	/// </summary>
	/// <remarks>
	/// Parses CLI arguments and provides access to partition, command, and subcommand.
	/// </remarks>
	class Command_Interpreter {
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="Command_Interpreter"/> class and parses the CLI arguments.
		/// </summary>
		/// <param name="argc">Argument count.</param>
		/// <param name="argv">Argument values.</param>
		Command_Interpreter(int argc, char* argv[]);

		/// <summary>
		/// Gets the top-level partition (e.g., "docker", "cloudflare").
		/// </summary>
		/// <returns>The partition string.</returns>
		std::string Get_Partition();

		/// <summary>
		/// Gets the main command (e.g., "start", "init", "remove").
		/// </summary>
		/// <returns>The command string.</returns>
		std::string Get_Command();

		/// <summary>
		/// Gets the subcommand, if any (e.g., "all", "policy", etc.).
		/// </summary>
		/// <returns>The subcommand string.</returns>
		std::string Get_SubCommand();

	private:
		/// <summary>
		/// The top-level partition (e.g., "docker", "cloudflare").
		/// </summary>
		std::string partition;

		/// <summary>
		/// The main command (e.g., "start", "add").
		/// </summary>
		std::string command;

		/// <summary>
		/// The optional secondary command.
		/// </summary>
		std::string subcommand;
	};

}
