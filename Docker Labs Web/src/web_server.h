#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <string>
#include <cstring>
#include <vector>

#define SOCKET_PATH "/tmp/docker_labs_web.sock"
#define HTTP_VERSION "HTTP/1.1 "


namespace Docker_Labs::Labs_Web {
	class HTTP_Response {
	public:
		HTTP_Response(int code, std::vector<std::string>& headers, std::string body);

		const char* Get_Response();
	private:
		const int response_code;
		const std::vector<std::string>& headers;
		const std::string response_body;
	};

	class Socket {
	public:
		Socket();
		int Listen();
		const char *socket_path = SOCKET_PATH;
	private:
		//nothing
	};

	class Connection {
	public:
		Connection(int client_socket);
		int respond();
	private:
		const int client_socket;
	};
}
