/**
 * global_cli.h
 * Global CLI command dispatch declarations.
 *
 * Declares handlers for global commands such as add, remove,
 * instantiate, and nuke, coordinating Docker and Cloudflare operations.
 */


#pragma once
#include "main.h"
#include "docker_labs/core/cloudflare_hook.h"
#include "docker_labs/core/docker_hook.h"

namespace Docker_Labs::Labs_CLI {

	// === GLOBAL CLI HANDLER ===
	// Routes high-level CLI commands to appropriate modules (Docker, Cloudflare, etc.)
	int Global_Handler(Labs_CLI::Command_Interpreter command, int argc, char* argv[]);


	// === CONTAINER MANAGEMENT HANDLERS ===

	// Handles creation and initialization of multiple containers based on a list of emails from a file.
	int Bulk_Instantiate_Handler(int argc, char* argv[]);

	// Handles initialization of a single container (networking, container setup, etc.)
	int Init_Container_Handler(int argc, char* argv[]);

	// Handles removal of a container, including optional cleanup from Docker and Cloudflare.
	int Remove_Container_Handler(int argc, char* argv[]);


	// === CLOUDFLARE DESTRUCTION HANDLER ===

	// Destroys all related Cloudflare resources (DNS records, Access apps, etc.)
	int Nuke_Cloudflare_Handler(int argc, char* argv[]);
}
