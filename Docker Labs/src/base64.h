#include <string>

class BASE64 {
private:
	// RFC 4648 Base64 Table (for encoding)
	static const char BASE64EncodingTable[64];

	// RFC 4648 Base64 Table (for decoding(remaked))
	// Char in ASCII(BASE64EncodedMessage) -> Reverse of BASE64EncodingTable
	static const int BASE64DecodingTable[131];

	static int	  binaryStringToDecimal(std::string binaryString);			// Binary style string -> Decimal integer
	static bool	  isStringASCII(std::string message);				// Is the given string fully consisted of ASCII characters?
public:
	static bool   isNumber(const std::string input);					// Check if the given string a real number
	static std::string lineSplitting(std::string BASE64message, int lenght);			// print with line splitting

	static std::string BASE64Encode(std::string message);					// Encode Base64 (integrated process)
	static std::string BASE64Decode(std::string message);					// Decode BASE64
};

