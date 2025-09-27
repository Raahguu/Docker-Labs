#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"

namespace Docker_Labs::Labs_CLI {

	/// <summary>
	/// GLOBAL CLI HANDLER
	/// <para>Routes high-level CLI commands to appropriate modules (Docker, Cloudflare, etc.).</para>
	/// </summary>

	/// <summary>
	/// Routes high-level CLI commands to the appropriate module (Docker, Cloudflare, etc.).
	/// </summary>
	/// <param name="command">The command interpreter instance.</param>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Global_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);

	/// <summary>
	/// CONTAINER MANAGEMENT HANDLERS
	/// <para>Handlers for container creation, initialization, and removal.</para>
	/// </summary>

	/// <summary>
	/// Handles creation and initialization of multiple containers based on a list of emails from a file.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Bulk_Instantiate_Handler(int argc, char* argv[]);

	/// <summary>
	/// Handles initialization of a single container, including networking and container setup.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Init_Container_Handler(int argc, char* argv[]);

	/// <summary>
	/// Handles removal of a container, including optional cleanup from Docker and Cloudflare.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Remove_Container_Handler(int argc, char* argv[]);

	/// <summary>
	/// CLOUDFLARE DESTRUCTION HANDLER
	/// <para>Destroys all related Cloudflare resources (DNS records, Access apps, etc.).</para>
	/// </summary>

	/// <summary>
	/// Destroys all related Cloudflare resources, such as DNS records and Access applications.
	/// </summary>
	/// <param name="argc">Argument count.</param>
	/// <param name="argv">Argument values.</param>
	/// <returns>Status code.</returns>
	int Nuke_Cloudflare_Handler(int argc, char* argv[]);
}
