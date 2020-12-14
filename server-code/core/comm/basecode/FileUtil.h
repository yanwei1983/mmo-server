#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>

#include "export_lua.h"

inline FILE* fopen_s(FILE** fp, const char* path, const char* mode)
{
    if(fp == nullptr)
        return nullptr;
    *fp = std::fopen(path, mode);
    return *fp;
}

constexpr unsigned char CODE_UTF_LEAD_0 = 0xefU;
constexpr unsigned char CODE_UTF_LEAD_1 = 0xbbU;
constexpr unsigned char CODE_UTF_LEAD_2 = 0xbfU;

inline void skip_utf8_bom(FILE* fp)
{
    if(fp == NULL)
        return;
    unsigned char ch1 = fgetc(fp);
    unsigned char ch2 = fgetc(fp);
    unsigned char ch3 = fgetc(fp);
    if(!(ch1 == CODE_UTF_LEAD_0 && ch2 == CODE_UTF_LEAD_1 && ch3 == CODE_UTF_LEAD_2)) //不等于BOM头时，重新回退到文件头
        fseek(fp, 0, SEEK_SET);
}

export_lua inline bool scan_dir(const std::string&                                                 parent_path,
                                const std::string&                                                 path,
                                bool                                                               bRecursive,
                                const std::function<void(const std::string&, const std::string&)>& func)
{
    std::string cur_dir = parent_path + "/" + path;
    DIR*        dp      = opendir(cur_dir.c_str());
    if(dp == nullptr)
    {
        return false;
    }

    struct dirent* result = readdir(dp);
    while(result != nullptr)
    {

        if(DT_DIR & result->d_type) // 判断下一级成员是否是目录
        {
            if(std::string(".") == result->d_name || std::string("..") == result->d_name)
            {
                result = readdir(dp);
                continue;
            }

            if(bRecursive)
            {
                scan_dir(cur_dir, result->d_name, bRecursive, func); // 递归调用自身，扫描下一级目录的内容
            }
        }
        else
        {
            func(cur_dir, result->d_name);
        }

        result = readdir(dp);
    }
    closedir(dp);

    return true;
}

inline void fixPath(std::string& path)
{
    if(path.empty())
    {
        return;
    }
    for(std::string::iterator iter = path.begin(); iter != path.end(); ++iter)
    {
        if(*iter == '\\')
        {
            *iter = '/';
        }
    }
    if(path.at(path.length() - 1) != '/')
    {
        path.append("/");
    }
}

inline bool isDirectory(std::string path)
{
#ifdef WIN32
    return PathIsDirectoryA(path.c_str()) ? true : false;
#else
    DIR* pdir = opendir(path.c_str());
    if(pdir == NULL)
    {
        return false;
    }
    else
    {
        closedir(pdir);
        pdir = NULL;
        return true;
    }
#endif
}

inline bool createRecursionDir(std::string path)
{
    if(path.length() == 0)
        return true;
    std::string sub;
    fixPath(path);

    std::string::size_type pos = path.find('/');
    while(pos != std::string::npos)
    {
        std::string cur = path.substr(0, pos - 0);
        if(cur.length() > 0 && !isDirectory(cur))
        {
            bool ret = false;
#ifdef WIN32
            ret = CreateDirectoryA(cur.c_str(), NULL) ? true : false;
#else
            ret = (mkdir(cur.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
            if(!ret)
            {
                return false;
            }
        }
        pos = path.find('/', pos + 1);
    }

    return true;
}

#endif /* FILEUTIL_H */
