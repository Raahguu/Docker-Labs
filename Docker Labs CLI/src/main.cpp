/**
 * global_cli.cpp
 * Implementation of global CLI command handlers.
 *
 * Implements command dispatching for global commands such as add,
 * remove, instantiate, and nuke, integrating Docker and Cloudflare functionality.
 */

#include "docker_labs/cli/main.h"
#include "docker_labs/cli/global_cli.h"
#include "docker_labs/cli/cloudflare_cli.h"
#include "docker_labs/cli/docker_cli.h"

using namespace Docker_Labs;

// ---------------------------------------------
// Main entry point
// Parses the top-level partition and dispatches command handling
// ---------------------------------------------
int main(int argc, char* argv[])
{
    // Parse the command line arguments into a Command_Interpreter instance
    Labs_CLI::Command_Interpreter command(argc, argv);

    // Dispatch to appropriate subcommand handler based on partition
    if (command.Get_Partition() == "cloudflare") {
        // Cloudflare related commands
        // Usage: ./labs-cli cloudflare <command> [<subcommand>]
        return Labs_CLI::Cloudflare::Command_Handler(command, argc, argv);
    }
    else if (command.Get_Partition() == "docker") {
        // Docker related commands
        return Labs_CLI::Docker::Command_Handler(command, argc, argv);
    }
    else {
        // Fallback to global command handler for other partitions
        return Labs_CLI::Global_Handler(command, argc, argv);
    }
}

// ---------------------------------------------
// Command_Interpreter constructor
// Parses the command line to extract partition, command, and subcommand
// ---------------------------------------------
Labs_CLI::Command_Interpreter::Command_Interpreter(int argc, char* argv[])
{
    if (argc < 2) {
        // Not enough arguments provided; print usage message
        std::cerr << "Usage: " << argv[0] << " <partition> [command] [subcommand]\n";
    }
    else {
        partition = argv[1];  // First argument after program name is partition

        int index = 2;

        // Check for first subcommand, if it exists and is not an option (no leading '-')
        if (argc > index && argv[index][0] != '-') {
            command = argv[index];
            ++index;
        }

        // Check for second subcommand, if it exists and is not an option
        if (argc > index && argv[index][0] != '-') {
            subcommand = argv[index];
            ++index;
        }
    }
}

// ---------------------------------------------
// Accessor for partition (top-level command group)
// ---------------------------------------------
std::string Labs_CLI::Command_Interpreter::Get_Partition() {
    return partition;
}

// ---------------------------------------------
// Accessor for first subcommand
// ---------------------------------------------
std::string Labs_CLI::Command_Interpreter::Get_Command() {
    return command;
}

// ---------------------------------------------
// Accessor for second subcommand
// ---------------------------------------------
std::string Labs_CLI::Command_Interpreter::Get_SubCommand() {
    return subcommand;
}
