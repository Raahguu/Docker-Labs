#!/bin/bash

# Define the installation directories
INSTALL_BIN_DIR="/usr/local/bin"
INSTALL_LIB_DIR="/usr/local/lib"

# Check if the script is run as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (use sudo)."
    exit 1
fi

# Create the installation directories if they do not exist
mkdir -p "$INSTALL_BIN_DIR"
mkdir -p "$INSTALL_LIB_DIR"

# Copy the binaries to the bin directory
echo "Installing binaries..."
cp -r ./bin/* "$INSTALL_BIN_DIR/"

# Copy the libraries to the lib directory
echo "Installing libraries..."
cp -r ./lib/* "$INSTALL_LIB_DIR/"

# Update the library cache
echo "Updating library cache..."
ldconfig

echo "Installation completed successfully."
