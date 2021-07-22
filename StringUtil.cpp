#include "stdafx.h"
#include "StringUtil.h"
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <stdarg.h>
    #include <langinfo.h>
#endif

namespace Base
{
    namespace StringUtil
    {

        std::string StringReplace(const std::string& strSource, 
            const std::string& strTarget /*= "'"*/, 
            const std::string& strEscape /*= "''"*/)
        {
            assert(!strTarget.empty());
            assert(!strEscape.empty());

            std::string strSqlString = strSource;
            size_t iOffset = strEscape.size() - strTarget.size() + 1;
            size_t iEscapeNum = strTarget.size();

            for (size_t upos =0;
                std::string::npos != (upos = strSqlString.find(strTarget,upos));
                upos += iOffset)
            {
                strSqlString.replace(upos, iEscapeNum, strEscape);
            }

            return strSqlString;

        }

        std::string str_fmt(const char* _Format, ...)
        {
            std::string _str;
            va_list marker = {};
            va_start(marker, _Format);
#ifdef _WIN32
            size_t num_of_chars = _vscprintf(_Format, marker);
            _str.resize(num_of_chars);
            vsprintf_s((char*)_str.c_str(), num_of_chars + 1, _Format, marker);
#else
            size_t num_of_chars = vsnprintf(0,0,_Format, marker);
            _str.resize(num_of_chars);
            vsnprintf((char*)_str.c_str(), num_of_chars + 1, _Format, marker);

#endif // 
            va_end(marker);
            return _str;
        }

//         int str_len(const std::string& str, STR_TYPE type /*= STR_TYPE::UTF8*/)
//         {
// #ifdef _WIN32
//             return MultiByteToWideChar(static_cast<int>(type),
//                 NULL,
//                 str.c_str(),
//                 static_cast<int>(str.length()),
//                 NULL,
//                 NULL
//             );
// #else
//             switch (type)
//             {
//             case STR_TYPE::UTF8:
//                 setlocale(LC_ALL, "zh_CN.UTF-8");
//             default:
//                 break;
//             }
// 
//             return mbstowcs(NULL, str.c_str(), 0);
// #endif
//         }


        std::string LowerCase(const std::string& str)
        {
            std::string str_temp = str;
            std::transform(str_temp.begin(), 
                str_temp.end(), 
                str_temp.begin(), 
                ::tolower);     // ::toupper

            return str_temp;
        }
    }
}



