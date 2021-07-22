#include "stdafx.h"
#include "Base64.h"
#include "modp_base64/modp_b64.h"


namespace Base
{
	namespace Encrypt
	{

		int Base64Encode(const char* in_str, int in_len, char* out_str)
		{
			return (int)modp_b64_encode(out_str, in_str, in_len);
		}

		bool Base64Encode(std::string& output, const std::string& input)
		{
			std::string str_tmp;
			str_tmp.resize(modp_b64_encode_len(input.size()));			// makes room for null byte
			int input_size = static_cast<int>(input.size());

			int out_put_size = (int)modp_b64_encode(&(str_tmp[0]), input.data(), input_size);

			if (out_put_size < 0)
			{
				return false;
			}

			str_tmp.resize(out_put_size);			// strips off nul byte
			output.swap(str_tmp);
			return true;
		}

		int Base64Decode(const char* in_str, int in_len, char* out_str)
		{
			return (int)modp_b64_decode(out_str, in_str, in_len);
		}

		bool Base64Decode(std::string& output, const std::string& input)
		{
			std::string str_tmp;
			str_tmp.resize(modp_b64_decode_len(input.size()));			// makes room for null byte
			int input_size = static_cast<int>(input.size());

			int out_put_size = (int)modp_b64_decode(&(str_tmp[0]), input.data(), input_size);

			if (out_put_size < 0)
			{
				return false;
			}

			str_tmp.resize(out_put_size);			// strips off nul byte
			output.swap(str_tmp);
			return true;
		}

	}
}