#include "StringAlgo.h"

#include <algorithm>
#include <codecvt>
#include <cstring>
#include <locale>
#include <string>

#include <iconv.h>
#include <regex.h>

#include "BaseCode.h"
#include "BaseType.h"
#include "FileUtil.h"

const unsigned char PL_utf8skip[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ascii */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* bogus */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* bogus */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* bogus */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* bogus */
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* scripts */
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* scripts */
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* cjk etc. */
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6  /* cjk etc. */
};

bool UTF8_IS_ASCII(unsigned char c)
{
    return c < 0x80;
}

bool ASCII_IS_CTRL(unsigned char c)
{
    return c < 32;
}

bool UTF8_IS_START(unsigned char c)
{
    return c >= 0xc0 && c <= 0xfd;
}

bool UTF8_IS_CONTINUATION(unsigned char c)
{
    return c >= 0x80 && c <= 0xbf;
}

bool UTF8_IS_CONTINUED(unsigned char c)
{
    return c & 0x80;
}

size_t utf8_length(const char* pUTF8, size_t nLen)
{
    if(nLen == 0)
        nLen = strlen(pUTF8);
    size_t nRet = 0;
    for(size_t i = 0; i < nLen;)
    {
        unsigned char ucUTF8 = *(pUTF8 + i);
        i += PL_utf8skip[ucUTF8];
        nRet++;
    }
    return nRet;
}

char* convert_enc(const char* encFrom, const char* encTo, char* pszBuffIn, size_t lenin, char* pszBuffout, size_t lenout)
{
    iconv_t c_pt;

    if((c_pt = iconv_open(encTo, encFrom)) == (iconv_t)-1)
    {
        fmt::print("iconv_open false: {} ==> {}" LINEEND, encFrom, encTo);
        return NULL;
    }
    iconv(c_pt, NULL, NULL, NULL, NULL);
    memset(pszBuffout, 0, lenout);
    int32_t ret = iconv(c_pt, &pszBuffIn, &lenin, &pszBuffout, &lenout);
    if(ret == -1)
    {
        return NULL;
    }
    iconv_close(c_pt);
    return pszBuffout;
}

/*
Code Points                    1st Byte   2nd Byte  3rd Byte  4th Byte
                    0aaaaaaa     0aaaaaaa
            00000bbbbbaaaaaa     110bbbbb  10aaaaaa
            ccccbbbbbbaaaaaa     1110cccc  10bbbbbb  10aaaaaa
  00000dddccccccbbbbbbaaaaaa     11110ddd  10cccccc  10bbbbbb  10aaaaaa
*/
bool is_utf8_none_control(const char* pszString, long nSize)
{
    bool        bIsUTF8 = true;
    const char* pStart  = pszString;
    const char* pEnd    = pszString + nSize;
    while(pStart < pEnd)
    {
        unsigned char n = *pStart;
        if(UTF8_IS_ASCII(n)) //小于0x80的是ASCII字符
        {
            if(ASCII_IS_CTRL(n)) //小于32的是控制字符
                return false;
            pStart += 1;
        }
        else if(UTF8_IS_START(n) == false) // 0x80~0xC0 之间的是无效UTF8字符
        {
            return false;
        }
        else
        {
            int len_need   = PL_utf8skip[n]; //总字符需求
            int extent_len = len_need - 1;   //额外需要的字符
            if(pStart >= pEnd - extent_len)
                return false; //字符数量不够
            //取每个字符前两位，必须都是0x80
            for(int i = 1; i < len_need; i++)
            {
                if((pStart[i] & (0xC0)) != 0x80)
                {
                    return false;
                }
            }
            pStart += len_need;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////
std::string url_encode(const char* pszStr)
{
    if(pszStr == nullptr)
        return "";

    std::string strOut   = "";
    char        szBuf[4] = {};
    for(int32_t i = 0; i < (int32_t)strlen(pszStr); i++)
    {
        memset(szBuf, 0, 4);
        if(isalnum((BYTE)pszStr[i]))
        {
            szBuf[0] = pszStr[i];
        }
        // 		else if (isspace((BYTE)pszStr[i]))
        else if(pszStr[i] == ' ')
        {
            szBuf[0] = '+';
        }
        else if(pszStr[i] == '.' || pszStr[i] == '-' || pszStr[i] == '_')
        {
            // 不需要处理
            szBuf[0] = pszStr[i];
        }
        else
        {
            szBuf[0] = '%';
            szBuf[1] = toHex((BYTE)pszStr[i] >> 4);
            szBuf[2] = toHex((BYTE)pszStr[i] & 0x0F);
            // 			safe_sprintf(szBuf, 4, "%%%02X", (BYTE)pszStr[i]);
        }
        strOut += szBuf;
    }
    return strOut;
}

//////////////////////////////////////////////////////////////////////
std::string url_decode(const char* pszStr)
{
    std::string strOut = "";
    if(!pszStr)
        return strOut;

    char    tmp[3] = {};
    int32_t i = 0, idx = 0, len = (int32_t)strlen(pszStr);

    while(i < len)
    {
        if(pszStr[i] == '%')
        {
            tmp[0]        = pszStr[i + 1];
            tmp[1]        = pszStr[i + 2];
            tmp[2]        = 0;
            int32_t nChar = 0;
            sscanf(tmp, "%X", &nChar);

            strOut += (char)nChar;
            i = i + 3;
        }
        else if(pszStr[i] == '+')
        {
            strOut += ' ';
            i++;
        }
        else
        {
            strOut += pszStr[i];
            i++;
        }
    }

    return strOut;
}

std::string get_fullpath(const std::string& szPath)
{
    if(szPath.empty())
        return "";
    if(szPath[0] == '/')
        return szPath;

    constexpr size_t _PATH_MAX = 1024;

    char szFull[_PATH_MAX * 2] = {};
    if(NULL == ::getcwd(szFull, _PATH_MAX))
        return "";

    std::string full_path(szFull);

    if(full_path[full_path.size() - 1] != '/')
        full_path += '/';
    std::string s = full_path + szPath;
    trim_path(s);
    return s;
}

int32_t CompareFileName(const std::string& szFile1, const std::string& szFile2)
{
    return szFile1.compare(szFile2);
}

class IllegalWordNode
{
public:
    IllegalWordNode(IllegalWordNode* pParent)
        : m_pParent(pParent)
        , m_bEnd(false)
    {
    }
    virtual ~IllegalWordNode()
    {
        for(NODE_SET::iterator it = setNodes.begin(); it != setNodes.end(); it++)
        {
            SAFE_DELETE(it->second);
        }
        setNodes.clear();
    }

    IllegalWordNode* GetNode(wchar_t wch) const
    {
        NODE_SET::const_iterator itFind = setNodes.find(wch);
        if(itFind == setNodes.end())
        {
            return NULL;
        }
        else
        {
            return itFind->second;
        }
    }

    IllegalWordNode* CreateNode(wchar_t wch)
    {
        NODE_SET::iterator itFind = setNodes.find(wch);
        if(itFind == setNodes.end())
        {
            IllegalWordNode* pNewNode = new IllegalWordNode(this);
            setNodes.insert(std::make_pair(wch, pNewNode));
            return pNewNode;
        }
        else
        {
            return itFind->second;
        }
    }

    bool HasMoreElement() const { return !setNodes.empty(); }

    void SetEnd() { m_bEnd = true; }

    bool IsEnd() const { return m_bEnd; }

    IllegalWordNode* GetParent() const { return m_pParent; }
    bool             IsRoot() const { return m_pParent == NULL; }

private:
    IllegalWordNode*                            m_pParent;
    typedef std::map<wchar_t, IllegalWordNode*> NODE_SET;
    NODE_SET                                    setNodes;
    bool                                        m_bEnd;
};

class IllegalWordRoot : public IllegalWordNode
{
public:
    IllegalWordRoot(const char* filename)
        : IllegalWordNode(NULL)
    {
        Init(filename);
    }
    virtual ~IllegalWordRoot() {}

    void Init(const char* filename)
    {

        // char szLine[1024] = "";
        int32_t     nIdx = 1;
        std::string szFileName(filename);
        FILE*       fp = NULL;
        while(fopen_s(&fp, szFileName.c_str(), "r") != nullptr)
        {
            char szLine[1024] = "";

            skip_utf8_bom(fp);

            while(EOF != fscanf(fp, "%s\n", szLine))
            {
                int32_t nLineSize = (int32_t)strlen(szLine);
                if(nLineSize > 0)
                {
                    wchar_t wszLine[2048] = {0};

                    if(0 == convert_enc("UTF-8", "UTF-32LE//IGNORE", (char*)szLine, nLineSize, (char*)wszLine, 2048 * sizeof(wchar_t)))
                    {
                    }
                    else
                    {
                        add(wszLine, wcslen(wszLine));
                    }
                }
            }

            fclose(fp);
            szFileName = fmt::format("pingbi/pingbi{}.txt", nIdx++);
        }
    }

    void add(const wchar_t* testString, size_t nSize)
    {
        //反序添加
        IllegalWordNode* pNode = NULL;
        for(int32_t i = (int32_t)nSize - 1; i >= 0; i--)
        {
            wchar_t str = testString[i];

            if(pNode == NULL)
            {
                pNode = CreateNode(str);
            }
            else
            {
                pNode = pNode->CreateNode(str);
            }
        }

        //添加结束标志
        if(pNode)
            pNode->SetEnd();
    }

    bool find(const wchar_t* testString, size_t nSize) const
    {
        if(testString == NULL)
            return false;
        bool                   bResult  = false;
        const IllegalWordNode* pNode    = this;
        const IllegalWordNode* pNewNode = NULL;
        //反序便利
        int32_t itBegin    = (int32_t)nSize - 1;
        int32_t itEnd      = -1;
        int32_t itSubStart = itEnd;
        int32_t itSubEnd   = itEnd;
        int32_t itLast     = itEnd;
        itLast++;
        for(int32_t it = itBegin; it != itEnd; it--)
        {
            wchar_t str = testString[it];
            if(str == L' ')
                continue;

            //匹配字符
            pNewNode = pNode->GetNode(str);
            if(pNewNode)
            {
                if(pNewNode->IsEnd())
                {
                    return true;
                }

                //否则记录开始匹配的下标
                if(itSubStart == itEnd)
                    itSubStart = it;

                //命中，该项没有后续项，代表全部命中
                if(pNewNode->HasMoreElement() == false && pNewNode->IsEnd())
                {
                    return true;
                }
                //如果当前已经是最后一个字符了
                else if(it == itLast)
                {
                    //上一个结束串位置，代表命中
                    if(itSubEnd != itEnd && itSubStart != itEnd)
                    {
                        //命中的是上一个
                        return true;
                    }
                    else
                    {
                        //否则从匹配开始的地方继续
                        it         = itSubStart;
                        itSubStart = itEnd;
                        itSubEnd   = itEnd;
                        pNode      = this;
                        continue;
                    }
                }

                pNode = pNewNode;
            }
            else
            {
                if(itSubEnd != itEnd)
                {
                    //命中的是上一个
                    return true;
                }
                if(itSubStart != itEnd)
                {
                    //否则从匹配开始的地方继续
                    it         = itSubStart;
                    itSubStart = itEnd;
                    itSubEnd   = itEnd;
                    pNode      = this;
                }
            }
        }
        return false;
    }

    bool replace(wchar_t* testString, size_t nSize) const
    {
        if(testString == NULL)
            return false;
        bool                   bResult  = false;
        const IllegalWordNode* pNode    = this;
        const IllegalWordNode* pNewNode = NULL;
        //反序便利
        int32_t itBegin    = (int32_t)nSize - 1;
        int32_t itEnd      = -1;
        int32_t itSubStart = itEnd;
        int32_t itSubEnd   = itEnd;
        int32_t itLast     = itEnd;
        itLast++;
        for(int32_t it = itBegin; it != itEnd; it--)
        {
            wchar_t str = testString[it];
            if(str == L' ')
                continue;

            //匹配字符
            pNewNode = pNode->GetNode(str);
            if(pNewNode)
            {
                if(pNewNode->IsEnd())
                {
                    itSubEnd = it;
                }

                //否则记录开始匹配的下标
                if(itSubStart == itEnd)
                    itSubStart = it;

                //命中，该项没有后续项，代表全部命中
                if(pNewNode->HasMoreElement() == false && pNewNode->IsEnd())
                {
                    int32_t itReplaceEnd = it;
                    int32_t itReplace    = itSubStart;
                    itReplaceEnd--;
                    for(; itReplace != itReplaceEnd; itReplace--)
                    {
                        testString[itReplace] = L'*';
                    }
                    bResult    = true;
                    itSubStart = itEnd;
                    itSubEnd   = itEnd;
                    pNode      = this;
                    continue;
                }
                //如果当前已经是最后一个字符了
                else if(it == itLast)
                {
                    //上一个结束串位置，代表命中
                    if(itSubEnd != itEnd && itSubStart != itEnd)
                    {
                        //命中的是上一个
                        int32_t itReplaceEnd = itSubEnd;
                        itReplaceEnd--;
                        for(int32_t itReplace = itSubStart; itReplace != itReplaceEnd; itReplace--)
                        {
                            testString[itReplace] = L'*';
                        }
                        bResult    = true;
                        it         = itSubEnd;
                        itSubStart = itEnd;
                        itSubEnd   = itEnd;
                        pNode      = this;
                        continue;
                    }
                    else
                    {
                        it         = itSubStart;
                        itSubStart = itEnd;
                        itSubEnd   = itEnd;
                        pNode      = this;
                        continue;
                    }
                }

                pNode = pNewNode;
            }
            else
            {
                if(itSubEnd != itEnd)
                {
                    //命中的是上一个
                    int32_t itReplaceEnd = itSubEnd;
                    itReplaceEnd--;
                    for(int32_t itReplace = itSubStart; itReplace != itReplaceEnd; itReplace--)
                    {
                        testString[itReplace] = L'*';
                    }
                    bResult    = true;
                    it         = itSubEnd;
                    itSubStart = itEnd;
                    itSubEnd   = itEnd;
                    pNode      = this;
                    continue;
                }
                if(itSubStart != itEnd)
                {
                    //否则从匹配开始的地方继续
                    it         = itSubStart;
                    itSubStart = itEnd;
                    itSubEnd   = itEnd;
                    pNode      = this;
                }
            }
        }

        return bResult;
    }
};

//////////////////////////////////////////////////////////////////////
bool find_name_error(const std::string& utf8)
{
    if(utf8.empty())
        return true;

    // 逐个字节过滤部分特殊字符，取ascii码33-126,128-254为合法字符
    if(is_utf8_none_control(utf8.c_str(), utf8.size()) == false)
        return true;

    if(regex_str_check(utf8) == true)
        return true;

    bool                                             bResult = true;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
    try
    {
        std::wstring wstr = conv1.from_bytes(utf8);
        bResult           = find_name_error(wstr);
    }
    catch(const std::range_error& e)
    {
        bResult = true;
    }

    return bResult;
}

//////////////////////////////////////////////////////////////////////
bool my_str_comp(const std::string& str1, const std::string& str2)
{
    return (str1 > str2);
}

bool replace_illegawords(std::string& utf8)
{
    if(utf8.empty())
        return false;
    bool                                             bResult = false;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
    try
    {
        std::wstring wstr = conv1.from_bytes(utf8);
        bResult           = replace_illegawords(wstr);
        if(bResult)
        {
            utf8 = conv1.to_bytes(wstr);
        }
    }
    catch(const std::range_error& e)
    {
        utf8.clear();
    }

    return bResult;
}

bool find_name_error(const std::wstring& wstr)
{
    if(wstr.empty())
        return true;

    static IllegalWordRoot s_IllegalWords("illegalnames.txt");
    bool                   bResult = s_IllegalWords.find(wstr.data(), wstr.size());

    return bResult;
}

bool replace_illegawords(std::wstring& wstr)
{
    if(wstr.empty())
        return false;

    static IllegalWordRoot s_IllegalWords("illegalwords.txt");
    bool                   bResult = s_IllegalWords.replace(wstr.data(), wstr.size());

    return bResult;
}

class CRegexIllegalStr
{
public:
    CRegexIllegalStr() { Init(); }
    virtual ~CRegexIllegalStr() { Destroy(); }
    //////////////////////////////////////////////////////////////////////////
    bool Init()
    {
        Destroy();

        char  szFileName[256] = "regexillegal.txt";
        FILE* fp              = NULL;
        if(fopen_s(&fp, szFileName, "r") != nullptr)
        {
            char szLine[1024] = "";
            skip_utf8_bom(fp);

            while(EOF != fscanf(fp, "%s\n", szLine))
            {
                int32_t nLineSize = (int32_t)strlen(szLine);
                if(nLineSize > 0 && szLine[nLineSize - 1] == '\r')
                {
                    szLine[nLineSize - 1] = '0';
                    --nLineSize;
                }
                if(nLineSize > 0)
                {
                    regex_t regex;
                    if(0 != regcomp(&regex, szLine, REG_EXTENDED | REG_NOSUB))
                    {
                        LOGERROR("Illegal regex patten: {}", szLine);
                        regfree(&regex);
                    }
                    else
                    {
                        m_setRegex.push_back(regex);
                    }
                }
            }
            fclose(fp);
        }
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Destroy()
    {
        std::deque<regex_t>::iterator it = m_setRegex.begin();
        for(; it != m_setRegex.end(); it++)
        {
            regex_t& regex = (regex_t&)(*it);
            regfree(&regex);
        }
        m_setRegex.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PattenMatch(const std::string& str)
    {
        if(str.empty())
            return false;
        std::deque<regex_t>::iterator it = m_setRegex.begin();
        for(; it != m_setRegex.end(); it++)
        {
            regex_t& regex = (regex_t&)(*it);
            if(0 == regexec(&regex, str.c_str(), 0, NULL, 0))
                return true;
        }
        return false;
    }

protected:
    std::deque<regex_t> m_setRegex;
};

/**
 * 基于正则表达式的名字合法性检查
 */
bool regex_str_check(const std::string& str)
{
    __ENTER_FUNCTION
    if(str.empty())
        return false;

    static CRegexIllegalStr s_regexIllegalStr;
    return !s_regexIllegalStr.PattenMatch(str);

    __LEAVE_FUNCTION
    return true;
}

bool regex_str_reload()
{
    __ENTER_FUNCTION
    static CRegexIllegalStr s_regexIllegalStr;
    s_regexIllegalStr.Init();
    __LEAVE_FUNCTION
    return true;
}