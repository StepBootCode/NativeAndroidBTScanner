#include "ConversionWchar.h"
#include <wchar.h>

WCHAR_T* ToV8StringJni(JNIEnv* env, IMemoryManager* m_iMemory, jstring jstr, int* lSize)
{
    WCHAR_T* ret = NULL;
    if (jstr)
    {
        JNIEnv* env = getJniEnv();
        *lSize = env->GetStringLength(jstr);
        const WCHAR_T* pjstr = env->GetStringChars(jstr, NULL);
        m_iMemory->AllocMemory((void**)&ret, (*lSize + 1) * sizeof(WCHAR_T));
        for (auto i = 0; i < *lSize; ++i)
            ret[i] = pjstr[i];
        ret[*lSize] = 0;
        env->ReleaseStringChars(jstr, pjstr);
    }
    return ret;
}

std::wstring ToWStringJni(JNIEnv* env, jstring jstr)
{
    std::wstring ret;
    if (jstr)
    {
        const jchar* jChars = env->GetStringChars(jstr, NULL);
        jsize jLen = env->GetStringLength(jstr);
        ret.assign(jChars, jChars + jLen);
        env->ReleaseStringChars(jstr, jChars);
    }
    return ret;
}

long jstring2v8string(JNIEnv* jenv, IMemoryManager* m_iMemory, jstring jstrIn, WCHAR_T** output) {

    if (jstrIn == NULL)
        return  0;

    jsize lengthInChars = jenv->GetStringLength(jstrIn);
    jsize size = (lengthInChars + 1) * sizeof(uint16_t);

    if (!m_iMemory || !m_iMemory->AllocMemory(reinterpret_cast<void**>(output), size)) {
        return 0;
    };

    const jchar* jStringChars = jenv->GetStringChars(jstrIn, 0);

    memcpy((*output), jStringChars, size);
    (*output)[lengthInChars] = 0;
    jenv->ReleaseStringChars(jstrIn, jStringChars);
    return size;
}


uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = static_cast<uint32_t>(::wcslen(Source) + 1);

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*)Source;
    uint32_t res = 0;

    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpShort = (WCHAR_T)*tmpWChar;
    }

    return res;
}


uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source) + 1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpWChar = (wchar_t)*tmpShort;
    }

    return res;
}


uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}

WcharWrapper::WcharWrapper(const wchar_t* str) :

#if defined(__APPLE__) || defined(__ANDROID__)

m_str_WCHAR(NULL),

#endif //__APPLE__ || __ANDROID__

m_str_wchar(NULL)
{
    if (str)
    {
        int len = static_cast<int>(wcslen(str));
        m_str_wchar = new wchar_t[len + 1];
        memset(m_str_wchar, 0, sizeof(wchar_t)* (len + 1));
        memcpy(m_str_wchar, str, sizeof(wchar_t)* len);
        
#if defined(__APPLE__) || defined(__ANDROID__)

        convToShortWchar(&m_str_WCHAR, m_str_wchar, len + 1);
        
#endif //__APPLE__ || __ANDROID__

    }
}


#if defined(__APPLE__) || defined(__ANDROID__)

WcharWrapper::WcharWrapper(const WCHAR_T* str) : m_str_WCHAR(NULL),
m_str_wchar(NULL)
{
    if (str)
    {
        int len = getLenShortWcharStr(str);
        m_str_WCHAR = new WCHAR_T[len + 1];
        memset(m_str_WCHAR,   0, sizeof(WCHAR_T) * (len + 1));
        memcpy(m_str_WCHAR, str, sizeof(WCHAR_T) * len);
        convFromShortWchar(&m_str_wchar, m_str_WCHAR);
    }
}

#endif //__APPLE__ || __ANDROID__


WcharWrapper::~WcharWrapper()
{
    
#if defined(__APPLE__) || defined(__ANDROID__)

    if (m_str_WCHAR)
    {
        delete[] m_str_WCHAR;
        m_str_WCHAR = NULL;
    }
    
#endif //__APPLE__ || __ANDROID__

    if (m_str_wchar)
    {
        delete[] m_str_wchar;
        m_str_wchar = NULL;
    }
}
