#pragma once

#include <string>

namespace Base
{
// 	class CBase64
// 	{
// 	public:
// 		CBase64();
// 		~CBase64();
// 
// 	public:
// 		static int Encrypt(char* out_str, const char* in_str, int in_len);
// 		static int Decrypt(char* out_str, const char* in_str, int in_len);
// 
// 	};

	namespace Encrypt
	{
		int Base64Encode(const char* in_str, int in_len, char* out_str);

		int Base64Decode(const char* in_str, int in_len, char* out_str);

		bool Base64Encode(std::string& output, const std::string& input);

		bool Base64Decode(std::string& output, const std::string& input);

	}


}
