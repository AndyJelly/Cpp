#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>

#ifndef _ERRNO_T_DEFINED
#define _ERRNO_T_DEFINED
typedef int errno_t;

#endif //_ERRNO_T_DEFINED

namespace Base
{
	namespace StringUtil
	{
		template <class T>
		inline std::string T2String(const T& source)
		{
			std::stringstream ss;
			ss << source;
			return ss.str();
		}

		// 特例化std::string 转std::string 
		template<>
		inline std::string T2String<std::string>(const std::string& source)
		{
			return source;
		}

		// std::string 转化成T类型，前提是T必须标准C++类型或者重载了转换符
		template<class T>
		inline T String2T(const std::string& str)
		{
			if (str.empty())
			{
				T v = T();
				return v;
			}

			T v;
			std::stringstream ss(str);
			ss >> v;
			return v;
		}

		//特例化string转string
		template<>
		inline std::string String2T<std::string>(const std::string& str)
		{
			return str;
		}

		// std::string 转c风格字符串
		template<size_t N>
		inline errno_t String2CStr(char(&szArray)[N], const char* pStr)
		{
			assert(!pStr);
			memset(szArray, 0, N);

#ifdef _WIN32
			return memcpy_s(szArray, N, pStr, strlen(pStr));
#else
			// 非windows，没有_s的函数
			int nCopy = strlen(pStr) > N ? N : strlen(pStr);
			memcpy(szArray, pStr, nCopy);
			return 0;
#endif
		}

		template<size_t N>
		inline errno_t String2CStr(char(&szArray)[N], const std::string& str)
		{
			memset(szArray, 0, N);

#ifdef _WIN32
			return memcpy_s(szArray, N, str.c_str(), str.size());
#else
			// 非windows，没有_s的函数
			int nCopy = str.size() > N ? N : str.size();
			memcpy(szArray, str.c_str(), nCopy);
			return 0;
#endif
		}

		// 字节流转std::string 
		template<size_t N>
		inline std::string Bytes2String(const unsigned char(&szArray)[N])
		{
			char szDuplicate[N + 1] = { 0 };
			memcpy(szDuplicate, szArray, N);
			return szDuplicate;
		}

		// std::string转字节流
		template<size_t N>
		inline void String2Bytes(unsigned char(&szArray)[N], const char* pStr)
		{
			assert(!pStr);
			memset(szArray, 0, N);

#ifdef _WIN32
			memcpy_s((char*)szArray, N, pStr, strlen(pStr));
#else
			// 非windows，没有_s的函数
			int nCopy = strlen(pStr) > N ? N : strlen(pStr);
			memcpy(szArray, pStr, nCopy);
#endif
		}

		template<size_t N>
		inline void String2Bytes(unsigned char(&szArray)[N], const std::string& str)
		{
			memset(szArray, 0, N);

#ifdef _WIN32
			memcpy_s((char*)szArray, N, str.c_str(), str.size());
#else
			// 非windows，没有_s的函数
			int nCopy = str.size() > N ? N : str.size();
			memcpy(szArray, str.c_str(), nCopy);
#endif
		}


        template<size_t N>
        inline bool BytesCmp(unsigned char(&szArray)[N], const char* pStr)
        {
            assert(!pStr);
            // strncmp  比较到第一个\0结束
            // 如果想比较全部，使用内存比较memcmp
            return strncmp((char*)szArray, pStr, N) == 0;
        }



        template<size_t N>
        inline void String2BytesEx(unsigned char(&szArray)[N], const std::string& str)
        {
            strncpy((char*)szArray, str.c_str(), N);
        }


        template<size_t N>
        inline void String2BytesEx(char(&szArray)[N], const std::string& str)
        {
            strncpy(szArray, str.c_str(), N);
        }

        // 切割字符串
        // 将1,2,3,4,5 转换成{1,2,3,4,5}
        template <typename T>
        void String2Array(std::vector<T>& vsValue, 
            const std::string& strArray, 
            char cSplit = ",")
        {
            vsValue.clear();

            size_t uPreIndx = 0;
            size_t uIndx = 0;
            size_t uLen = 0;

            while ((uIndx = strArray.find_first_of(cSplit,uPreIndx)) != std::string::npos)
            {
                if ((uLen = uIndx - uPreIndx) != 0)
                {
                    vsValue.push_back(String2T<T>(strArray.substr(uPreIndx, uLen)));
                }
                uPreIndx = uIndx + 1;
            }

            if (uPreIndx < strArray.size())
            {
                vsValue.push_back(String2T<T>(strArray.substr(uPreIndx).c_str()));
            }
        }

        // 将{1,2,3,4,5} 转换成1,2,3,4,5
        template<typename T>
        void Array2String(std::string& strArray, 
            const std::vector<T>& vsValue,
            char cSplit = ',')
        {
            if (vsValue.empty())
            {
                return;
            }

            strArray.clear();
            strArray.reserve(vsValue.size() * 3);

            for (typename std::vector<T>::const_iterator it = vsValue.begin();;)
            {
                strArray += T2String(*it);
                if (++it != vsValue.end())
                {
                    strArray += cSplit;
                }
                else
                {
                    break;
                }
            }
        }


        // 替换字符串中特定字符
        std::string StringReplace(const std::string& strSource,
            const std::string& strTarget = "'",
            const std::string& strEscape = "''");

        // 字符串格式化
        std::string str_fmt(const char* _Format, ...);

        // 计算字符串中字符的长度（非字节长度）
        enum class STR_TYPE
        {
            ACP = 0,
            OEMCP = 1,
            MACCP = 2,
            THREAD_ACP = 3,
            SYMBOL = 42,
            UTF7 = 65000,
            UTF8 = 65001,
        };

		int str_len(const std::string& str, STR_TYPE type = STR_TYPE::UTF8)
		{
#ifdef _WIN32
			return MultiByteToWideChar(static_cast<int>(type),
				NULL,
				str.c_str(),
				static_cast<int>(str.length()),
				NULL,
				NULL
			);
#else
			switch (type)
			{
			case STR_TYPE::UTF8:
				setlocale(LC_ALL, "zh_CN.UTF-8");
			default:
				break;
			}

			return mbstowcs(NULL, str.c_str(), 0);
#endif
		}

        std::string LowerCase(const std::string& str);

	} 
}