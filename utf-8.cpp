// utf-8Demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string.h>
#include "windows.h"
#include <assert.h>
#include <iostream>
#include <memory>

/*
打开文件乱码的出现是由于，打开文件盒保存文件的编码方式不一致所导致的。
计算机中所有信息最终都表示为一个字节，1字节=8 bits，所以一个字节可以有256中表现形式（即可以表示256中字符），
远不能够满足实际需求；所以采用Unicode字符集，将所有符号都纳入其中，对于每一个符号都给与一个独一无二的编码。
所以这里需要强调的是Unicode只是一个字符集，将所有的符号都对应的用若干个字符去表示；
但是如何区分Unicode和ASCII（一个字节表示一个字符），即比如说一个三字节，我们如何知道他是表示一个字符还是三个字符；
这就需要UTF-8，UTF-8只是Unicode的一种实现方式。
*/

/*
UTF-8的编码规则很简单，只有二条：
1）对于单字节的符号，字节的第一位设为0，后面7位为这个符号的unicode码。因此对于英语字母，UTF-8编码和ASCII码是相同的。
2）对于n字节的符号（n>1），第一个字节的前n位都设为1，第n+1位设为0，后面字节的前两位一律设为10。剩下的没有提及的二进制位，全部为这个符号的unicode码。
下表总结了编码规则，字母x表示可用编码的位。

Unicode符号范围 | UTF-8编码方式
(十六进制) | （二进制）
--------------------+---------------------------------------------
0000 0000-0000 007F | 0xxxxxxx
0000 0080-0000 07FF | 110xxxxx 10xxxxxx
0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

UTF-8：编码是六个字节"EF BB BF E4 B8 A5"，前三个字节"EF BB BF"表示这是UTF-8编码，后三个"E4B8A5"就是"严"的具体编码，它的存储顺序与编码顺序是一致的。
*/

#define MIN(a,b)  ((a) > (b) ? (b) : (a))


int get_bytenum_1(char ch)
{
    //通过判断字节所对应的整型值，判断该字符占多少个字节
    int nCode = (unsigned char)ch;   //0xxxxxxx   表示占一个字节（0-127）0000 0000-0111 1111
    if (nCode < 128)
        return 1;
    else if (nCode >= 192 && nCode <= 223)    // 110xxxxx 表示占两个字节  1100 0000-1101 1111（192-223）
        return 2;
    else if (nCode >= 224 && nCode <= 239)
        return 3;
    else if (nCode >= 240 && nCode <= 247)
        return 4;
    else if (nCode >= 248 && nCode <= 251)
        return 5;
    else if (nCode >= 252 && nCode <= 253)
        return 6;
    else  //一个字节表示从1000 0000(128)-1011 1111(191),表示该字节不是非u8的首字节
        return 0;
}


char * u8_safe_cut(char * u8)
{    
    if (u8)
    {
        std::cout << u8 << std::endl;
    }
    int cnt = 0;
    int pos = strlen(u8) - 1;
    while (1)
    {
        int ret = get_bytenum_1(u8[pos]);
        if (ret == 1)//ansi 字符   对于最后一个字符是ASCII字符（只占一个字节）不用考虑截断问题
        {
            break;
        }
        else if (ret == 0)//u8非首字节  对于最后一位字节不是u8首字节的，继续向前取
        {
            cnt++;
        }
        else //u8首字节    对于占多个字节的字符，
        {
            cnt++;
            if (cnt != ret)      //对于u8字符，占用多个字节时，如果后面剩下的字节数，正好完整的存放一个字符则不截断，否则cnt != ret，进行截断
            {
                u8[pos] = 0;
            }
            break;
        }

        pos--;  //对于最后一位字节不是u8首字节的，继续向前取
    }

    return u8;
}

template <size_t N>
inline void String2CStr(char(&szArray)[N], const char* pStr)
{
    assert(pStr != NULL); // 不能为NULL
    memset(szArray, 0, N);
    int min_len = MIN(N - 1, strlen(pStr));
    memcpy_s(szArray, min_len, pStr, min_len);
    u8_safe_cut(szArray);
}

 char* OpenU8File()
{
    FILE* pfile = fopen("UTF-8.txt", "r");
    if ( !pfile )
    {
        perror("Error opening file"); 
        return nullptr;
    }

    size_t fsize = 0;
    //通过fseek 和ftell 获取文件的大小
    // obtain file size:
    fseek(pfile, 0, SEEK_END);
    fsize = static_cast<size_t>(ftell(pfile));

    //重新将文件位置指针移到文件首的位置
    fseek(pfile, 0, SEEK_SET);

    //new出来一个buf 存储文件中的内容，并使用memset将其初始化
    std::unique_ptr<char> pBuf(new char[fsize + 1]);
    memset(pBuf.get(), 0, fsize + 1);

    size_t read_size = fread(pBuf.get(), 1, fsize, pfile);
    fclose(pfile);
    if (read_size == fsize)
    {
        return pBuf.get();
    }
    else
    {
        std::cout << "文件读取不完整" << std::endl;
        return pBuf.get();
    }

    /*
    其中bar是一个智能指针，p是一个普通指针：

    p = bar.get(); 后，bar并非被释放，也就相当于指针p和智能指针bar共同管理一个对象，所以就*p做的一切，都会反应到bar指向的对象上。
    */

}

int _tmain(int argc, _TCHAR* argv[])
{
    //char* ch1 = "严严q严严严严严严严严";     //这里输入的并不是UTF-8形式编码的，而是采用ANSI编码

    //std::cout << strlen(ch1) << std::endl;
    FILE* pfile = fopen("UTF-8.txt", "r");
    if (!pfile)
    {
        perror("Error opening file");
        return -1;
    }

    size_t fsize = 0;
    //通过fseek 和ftell 获取文件的大小
    // obtain file size:
    fseek(pfile, 0, SEEK_END);
    fsize = static_cast<size_t>(ftell(pfile));

    //重新将文件位置指针移到文件首的位置
    fseek(pfile, 0, SEEK_SET);

    //new出来一个buf 存储文件中的内容，并使用memset将其初始化
    std::unique_ptr<char> pBuf(new char[fsize + 1]);
    memset(pBuf.get(), 0, fsize + 1);

    size_t read_size = fread(pBuf.get(), 1, fsize, pfile);
    fclose(pfile);


    std::cout << pBuf.get() << " length: " << strlen(pBuf.get()) << std::endl;
    char ch2[9] = {0};

    String2CStr(ch2, pBuf.get());

    std::cout << ch2 <<" length: "<< strlen(ch2) << std::endl;
    system("pause");

    return 0;
}

