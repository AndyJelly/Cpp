// utf-8Demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string.h>
#include "windows.h"
#include <assert.h>

int get_bytenum_1(char ch)
{
    int nCode = (unsigned char)ch;
    if (nCode < 128)
        return 1;
    else if (nCode >= 192 && nCode <= 223)
        return 2;
    else if (nCode >= 224 && nCode <= 239)
        return 3;
    else if (nCode >= 240 && nCode <= 247)
        return 4;
    else if (nCode >= 248 && nCode <= 251)
        return 5;
    else if (nCode >= 252 && nCode <= 253)
        return 6;
    else
        return 0;
}


char * u8_safe_cut(char * u8)
{
    int cnt = 0;
    int pos = strlen(u8) - 1;
    while (1)
    {
        int ret = get_bytenum_1(u8[pos]);
        if (ret == 1)//ansi 字符
        {
            break;
        }
        else if (ret == 0)//u8非首字节
        {
            cnt++;
        }
        else //u8首字节
        {
            cnt++;
            if (cnt != ret)
            {
                u8[pos] = 0;
            }
            break;
        }

        pos--;
    }

    return u8;
}


#define MIN(a,b)  ((a) > (b) ? (b) : (a))


template <size_t N>
inline void String2CStr(char(&szArray)[N], const char* pStr)
{
    assert(pStr != NULL); // 不能为NULL
    memset(szArray, 0, N);
    int min_len = MIN(N - 1, strlen(pStr));//
    memcpy_s(szArray, min_len, pStr, min_len);
    u8_safe_cut(szArray);
}

int _tmain(int argc, _TCHAR* argv[])
{
    char* ch1 = "严严严严严严严严严严";
    char ch2[4] = {0};

    String2CStr(ch2, ch1);

    
    system("pause");

	return 0;
}

