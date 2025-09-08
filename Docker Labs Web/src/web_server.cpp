#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "web_server.h"

using namespace Docker_Labs;

Labs_Web::HTTP_Response::HTTP_Response(int code, std::vector<std::string>& headers, std::string body) 
	: response_code(code), headers(headers), response_body(body)
{
	
}

const char* Labs_Web::HTTP_Response::Get_Response() {
	std::vector<std::string> core_headers = {
		"Server: DockerLabsWeb/0.1",
		"Content-Length: " + std::to_string(response_body.size()),
	};
	std::string response_str = HTTP_VERSION + std::to_string(response_code) + " OK\r\n";
	for (std::string header : headers) {
	        response_str+= header + "\r\n";
	}
	for (std::string header : core_headers) {
		response_str+= header + "\r\n";
	}
	response_str+= "\r\n";
	response_str+= response_body;
	response_str+= "\r\n";

	char* response = new char[response_str.size()+1];
	std::strcpy(response, response_str.c_str());

	return response;
}

Labs_Web::Socket::Socket() {
}

int Labs_Web::Socket::Listen() {
	int server_socket;
	int client_socket;
	struct sockaddr_un server_addr;

	server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_socket < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

	unlink(SOCKET_PATH);

	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Error binding socket" << std::endl;
		close(server_socket);
		return 1;
	}

	listen(server_socket, 5);
	std::cout << "Server listening on " << SOCKET_PATH << "..." << std::endl;

	while (true) {
		client_socket = accept(server_socket, nullptr, nullptr);
		if (client_socket < 0) {
			std::cerr << "Error accepting connection" << std::endl;
			continue;
		}

		Labs_Web::Connection client = Labs_Web::Connection(client_socket);
		client.respond();
	}
}

Labs_Web::Connection::Connection(int client_socket)
	: client_socket(client_socket)
{

}

int Labs_Web::Connection::respond() {
	std::vector<std::string> headers = std::vector<std::string>();
	headers.push_back("Content-Type: text/html");
	Labs_Web::HTTP_Response response = Labs_Web::HTTP_Response(200, headers, 
			"<html>\r\n"
			"    <body>\r\n"
			"        Hello\r\n"
			"    </body>\r\n"
			"</html>\r\n"
			);
	const char* message = response.Get_Response();	
	std::cout << "Responding with:" << std::endl << std::endl;
	std::cout << message << std::endl;
	send(client_socket, message, strlen(message), 0);
	close(client_socket);
	return 0;
}
