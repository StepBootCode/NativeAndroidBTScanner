#include <jni.h>
#include <string>
#include "../../../../../BTScaner/BTScaner.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_ru_bootcode_nativetemplate_MainActivity_hello(JNIEnv *env, jobject thiz) {
    std::string hello = "Its ru_bootcode_btscaner";
    return env->NewStringUTF(hello.c_str());
}
