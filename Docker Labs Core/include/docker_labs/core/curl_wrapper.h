#pragma once
#include <curl/curl.h>
#include <string>
#include <vector>

namespace Docker_Labs::Labs_Core {

	/// <summary>
	/// Provides a thin wrapper around libcurl for HTTP and socket-based web requests.
	/// Offers convenience methods for executing HTTP requests (GET, POST, PUT, etc.) with customizable headers and payloads, including support for UNIX socket requests.
	/// </summary>
	class Curl_Wrapper {
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="Curl_Wrapper"/> class.
		/// </summary>
		Curl_Wrapper();

		/// <summary>
		/// Executes an HTTP GET request.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Get(const std::string& url, std::vector<std::string>& headers);

		/// <summary>
		/// Executes an HTTP DELETE request.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Delete(const std::string& url, std::vector<std::string>& headers);

		/// <summary>
		/// Executes an HTTP PUT request.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="data">Payload to send in the request body.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Put(const std::string& url, const std::string& data, const std::vector<std::string>& headers);

		/// <summary>
		/// Executes an HTTP POST request.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="data">Payload to send in the request body.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers);

		/// <summary>
		/// Executes an HTTP PATCH request.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="data">Payload to send in the request body.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Patch(const std::string& url, const std::string& data, const std::vector<std::string>& headers);

		/// <summary>
		/// Executes an HTTP request over a UNIX socket.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="data">Payload to send in the request body.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <param name="request_type">HTTP method (e.g., "GET", "POST").</param>
		/// <param name="socket">Path to the UNIX socket.</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Socket_Request(
			const std::string& url,
			const std::string& data,
			const std::vector<std::string>& headers,
			std::string& request_type,
			std::string socket
		);

		/// <summary>
		/// libcurl write callback for handling response data.
		/// </summary>
		/// <param name="contents">Pointer to the delivered data.</param>
		/// <param name="size">Size of a data chunk.</param>
		/// <param name="nmemb">Number of data chunks.</param>
		/// <param name="userp">Pointer to user data (usually a string buffer).</param>
		/// <returns>Number of bytes handled.</returns>
		static std::size_t WriteCallback(char* contents, std::size_t size, std::size_t nmemb, std::string* userp);

		/// <summary>
		/// Raw libcurl handle for direct access.
		/// </summary>
		CURL* curl;

		/// <summary>
		/// Result code from the last libcurl operation.
		/// </summary>
		CURLcode res;

	private:
		/// <summary>
		/// Internal helper for executing a web request with data payload.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="data">Payload to send in the request body.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <param name="request_type">HTTP method (e.g., "GET", "POST").</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Web_Request(
			const std::string& url,
			const std::string& data,
			const std::vector<std::string>& headers,
			std::string& request_type
		);

		/// <summary>
		/// Internal helper for executing a web request without data payload.
		/// </summary>
		/// <param name="url">The target URL.</param>
		/// <param name="headers">Headers to include in the request.</param>
		/// <param name="request_type">HTTP method (e.g., "GET", "DELETE").</param>
		/// <returns>Tuple containing HTTP status code and response body.</returns>
		std::tuple<long, std::string> Web_Request(
			const std::string& url,
			const std::vector<std::string>& headers,
			std::string& request_type
		);
	};

} // namespace Docker_Labs::Labs_Core

