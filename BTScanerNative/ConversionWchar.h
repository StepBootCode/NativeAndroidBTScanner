#include <string>
#include "../include/types.h"
#include "../include/IMemoryManager.h"
#include "../jni/jnienv.h"

WCHAR_T* ToV8StringJni(JNIEnv* env, IMemoryManager* m_iMemory, jstring jstr, int* lSize);
std::wstring ToWStringJni(JNIEnv* env, jstring jstr);
long jstring2v8string(JNIEnv* jenv, IMemoryManager* m_iMemory, jstring jstrIn, WCHAR_T** output);
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);

class WcharWrapper
{
public:

    WcharWrapper(const WCHAR_T* str);
    WcharWrapper(const wchar_t* str);
    ~WcharWrapper();
    operator const WCHAR_T*() { return m_str_WCHAR; }
    operator WCHAR_T*() { return m_str_WCHAR; }
    operator const wchar_t*() { return m_str_wchar; }
    operator wchar_t*() { return m_str_wchar; }
private:

    WcharWrapper& operator = (const WcharWrapper& other) { return *this; };
    WcharWrapper(const WcharWrapper& other) {};
private:

    WCHAR_T* m_str_WCHAR;
    wchar_t* m_str_wchar;
};
