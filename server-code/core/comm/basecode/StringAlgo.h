#ifndef STRINGALGO_H
#define STRINGALGO_H

#include <algorithm>
#include <string>
#include <vector>
#include <functional>
#include "export_lua.h"
//跳过utf8_bom头
export_lua void skip_utf8_bom(FILE* fp);
//获取utf8文字长度
export_lua size_t utf8_length(const char* pUTF8, size_t nLen = 0);
//编码转换
export_lua char* convert_enc(const char* encFrom, const char* encTo, char* pszBuffIn, size_t lenin, char* pszBuffout, size_t lenout);
//检查控制字符
export_lua bool is_utf8_none_control(const char* pszString, long nSize);

export_lua inline std::string& ltrim(std::string& ss);
export_lua inline std::string& rtrim(std::string& ss);
export_lua inline std::string& trim(std::string& st);

export_lua inline std::string ltrim_copy(std::string ss);
export_lua inline std::string rtrim_copy(std::string ss);
export_lua inline std::string trim_copy(std::string st);

export_lua inline std::string& lower_cast(std::string& st);
export_lua inline std::string  lower_cast_copy(std::string st);
export_lua inline std::string& upper_cast(std::string& st);
export_lua inline std::string  upper_cast_copy(std::string st);

export_lua inline std::vector<std::string>      split_string(const std::string& str, const std::string& delimiters);
export_lua inline std::vector<std::string_view> split_string_view(const std::string& str, const std::string& delimiters);

export_lua inline std::string replace_str(std::string& strSource, const std::string& strRepl, const std::string& strNew);
export_lua inline std::string replace_str_copy(std::string strSource, const std::string& strRepl, const std::string& strNew);

export_lua std::string url_decode(const char* pszStr);
export_lua std::string url_encode(const char* pszStr);

export_lua bool replace_illegawords(std::wstring& wstr);
export_lua bool replace_illegawords(std::string& utf8);
export_lua bool find_name_error(const std::wstring& wstr);
export_lua bool find_name_error(const std::string& utf8);

export_lua bool regex_str_check(const std::string& str);
export_lua bool regex_str_reload();

export_lua inline std::string string_concat(const std::vector<std::string>& vecStr,
                                            const std::string&              delimiters,
                                            const std::string&              pre,
                                            const std::string&              post);

////////////////////////////////////////////////////////
// filepath process

export_lua inline std::string get_filename_from_fullpath(const std::string& szFullPath)
{
    std::string::size_type pos = szFullPath.find_last_of("\\/");
    std::string            szName;
    if(pos != std::string::npos)
    {
        szName = szFullPath.substr(pos + 1);
    }
    else
        szName = szFullPath;
    return szName;
}

export_lua inline std::string get_filename_without_ext(const std::string& szFile)
{
    std::string::size_type pos1 = szFile.find_last_of("/\\");
    std::string::size_type pos2 = szFile.rfind('.');
    pos1                        = pos1 == std::string::npos ? 0 : pos1 + 1;
    if(pos2 == std::string::npos)
        pos2 = szFile.size();
    if(pos2 < pos1)
        pos2 = pos1;
    return szFile.substr(pos1, pos2 - pos1);
}

export_lua inline std::string get_file_ext(const std::string& szFile)
{
    std::string            szName = get_filename_from_fullpath(szFile);
    std::string::size_type nPos   = szName.rfind('.');
    if(nPos != std::string::npos)
        ++nPos;
    else
        nPos = szName.size();

    return szName.substr(nPos);
}

export_lua inline std::string get_basepath(const std::string& szFullPath)
{
    std::string::size_type pos = szFullPath.find_last_of("/\\");
    std::string            szName;
    if(pos != std::string::npos)
    {
        szName = szFullPath.substr(0, pos);
    }
    else
        szName = szFullPath;
    return szName;
}

template<typename OI, typename II>
OI remove_sub_sequence(OI first1, OI last1, II first2, II last2)
{
    OI            r1 = std::search(first1, last1, first2, last2);
    OI            w = r1, r2 = r1;
    const auto nDiff = std::distance(first2, last2);
    while(r2 != last1)
    {
        std::advance(r1, nDiff);                    // ignore [first2, last2)
        r2 = std::search(r1, last1, first2, last2); // find next
        while(r1 != r2)
        { // write element
            *w = *r1;
            ++w, ++r1;
        }
    }
    return w;
}

export_lua inline void trim_path(std::string& szPath)
{
    char c1 = '\\', c2 = '/';
    std::replace(szPath.begin(), szPath.end(), c1, c2);
    std::string            szDogDog = std::string("..") + c2;
    std::string::size_type pos      = szPath.find(szDogDog);
    for(; pos != std::string::npos && pos > 2; pos = szPath.find(szDogDog))
    {
        std::string::size_type nLast = szPath.rfind(c2, pos - 2);
        szPath.erase(nLast, pos + szDogDog.size() - nLast - 1);
    }
    std::string szDS = std::string(".") + c2;

    szPath.erase(remove_sub_sequence(szPath.begin(), szPath.end(), szDS.begin(), szDS.end()), szPath.end());
}

export_lua std::string get_fullpath(const std::string& szPath);

//////////////////////////////////////////////////////////////////////////
inline std::string replace_str(std::string& strSource, const std::string& strRepl, const std::string& strNew)
{
    std::string::size_type pos = 0;
    while((pos = strSource.find(strRepl, pos)) != std::string::npos)
    {
        strSource.replace(pos, strRepl.length(), strNew);
        pos += strNew.length();
    }
    return strSource;
}

inline std::string replace_str_copy(std::string strSource, const std::string& strRepl, const std::string& strNew)
{
    std::string::size_type pos = 0;
    while((pos = strSource.find(strRepl, pos)) != std::string::npos)
    {
        strSource.replace(pos, strRepl.length(), strNew);
        pos += strNew.length();
    }
    return strSource;
}

inline std::string& ltrim(std::string& ss)
{
    std::string::iterator p = std::find_if_not(ss.begin(), ss.end(), [](unsigned char c) { return std::isspace(c); });
    ss.erase(ss.begin(), p);
    return ss;
}

inline std::string& rtrim(std::string& ss)
{
    std::string::reverse_iterator p = std::find_if_not(ss.rbegin(), ss.rend(), [](unsigned char c) { return std::isspace(c); });
    ss.erase(p.base(), ss.end());
    return ss;
}

inline std::string& trim(std::string& st)
{
    ltrim(rtrim(st));
    return st;
}

inline std::string ltrim_copy(std::string ss)
{
    std::string::iterator p = std::find_if_not(ss.begin(), ss.end(), [](unsigned char c) { return std::isspace(c); });
    ss.erase(ss.begin(), p);
    return ss;
}

inline std::string rtrim_copy(std::string ss)
{
    std::string::reverse_iterator p = std::find_if_not(ss.rbegin(), ss.rend(), [](unsigned char c) { return std::isspace(c); });
    ss.erase(p.base(), ss.end());
    return ss;
}

inline std::string trim_copy(std::string st)
{
    ltrim(rtrim(st));
    return st;
}

inline std::string& ltrim_if(std::string& ss, const std::function<bool(unsigned char)>& func)
{
    std::string::iterator p = std::find_if(ss.begin(), ss.end(), func);
    ss.erase(ss.begin(), p);
    return ss;
}

inline std::string& rtrim_if(std::string& ss, const std::function<bool(unsigned char)>& func)
{
    std::string::reverse_iterator p = std::find_if(ss.rbegin(), ss.rend(), func);
    ss.erase(p.base(), ss.end());
    return ss;
}

inline std::string& trim_if(std::string& st, const std::function<bool(unsigned char)>& func)
{
    ltrim_if(rtrim_if(st, func), func);
    return st;
}

inline std::string& lower_cast(std::string& st)
{
    std::transform(st.begin(), st.end(), st.begin(), [](unsigned char c) { return std::tolower(c); });
    return st;
}

inline std::string lower_cast_copy(std::string st)
{
    std::transform(st.begin(), st.end(), st.begin(), [](unsigned char c) { return std::tolower(c); });
    return st;
}

inline std::string& upper_cast(std::string& st)
{
    std::transform(st.begin(), st.end(), st.begin(), [](unsigned char c) { return std::toupper(c); });
    return st;
}

inline std::string upper_cast_copy(std::string st)
{
    std::transform(st.begin(), st.end(), st.begin(), [](unsigned char c) { return std::toupper(c); });
    return st;
}

inline std::vector<std::string> split_string(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> v;
    std::string::size_type   start = 0;
    auto                     pos   = str.find_first_of(delimiters, start);
    while(pos != std::string::npos)
    {
        if(pos != start) // ignore empty tokens
            v.emplace_back(str, start, pos - start);
        start = pos + 1;
        pos   = str.find_first_of(delimiters, start);
    }
    if(start < str.length())                              // ignore trailing delimiter
        v.emplace_back(str, start, str.length() - start); // add what's left of the string
    return v;
}

inline std::vector<std::string_view> split_string_view(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string_view> v;
    std::string::size_type        start = 0;
    auto                          pos   = str.find_first_of(delimiters, start);
    while(pos != std::string::npos)
    {
        if(pos != start) // ignore empty tokens
            v.emplace_back(std::string_view{str.c_str() + start, pos - start});
        start = pos + 1;
        pos   = str.find_first_of(delimiters, start);
    }
    if(start < str.length())                                                         // ignore trailing delimiter
        v.emplace_back(std::string_view{str.c_str() + start, str.length() - start}); // add what's left of the string
    return v;
}

template<class T>
inline std::string string_concat_impl(const T& vecStr, const std::string& delimiters, const std::string& pre, const std::string& post)
{
    std::string result;
    int32_t     i = 0;
    for(const auto& v: vecStr)
    {
        if(result.empty() == false)
            result += delimiters;

        result += pre + v + post;
    }
    return result;
}

inline std::string string_concat(const std::vector<std::string>& vecStr,
                                 const std::string&              delimiters,
                                 const std::string&              pre,
                                 const std::string&              post)
{
    return string_concat_impl(vecStr, delimiters, pre, post);
}

#endif /* STRINGALGO_H */
