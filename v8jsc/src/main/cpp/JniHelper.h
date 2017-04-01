/*
 * JniHelper.h
 * 主要用来缓存JNIEnv
 *  Created on: 2014年8月28日
 *      Author: liaomin
 */

#ifndef __ANDROID_JNI_HELPER_H__
#define __ANDROID_JNI_HELPER_H__

#include <jni.h>
#include <string>
#include "JavaMethodInfo.h"

namespace FQ {
class JavaObject;

class JniHelper
{
public:
    static void SetJavaVM(JavaVM *javaVM);
    static JavaVM* GetJavaVM();
    static JNIEnv* GetEnv();
    static bool GetJavaMethodInfo(JavaMethodInfo &methodinfo,const char *className,const char *methodName,const char *parameterTypes);
    static bool GetJavaMethodInfo(JavaMethodInfo &methodinfo,jobject object,const char *methodName,const char *parameterTypes);
    static bool GetJavaStaticMethodInfo(JavaMethodInfo &methodinfo,const char *className,const char *methodName,const char *parameterTypes);
    static jobject NewJaveObject(const char *className,const char *parameterTypes,...);
    static jobject NewJaveObjectA(const char *className,const char *parameterTypes,jvalue *args);
    static jobject NewJaveObjectV(const char *className,const char *parameterTypes,va_list args);
    static jobject GetJavaObject(JavaObject *cobject);
    static void CacheJavaObject(JavaObject *cobject,jobject obj);
    static void RemoveCache(JavaObject *cobject);
private:
    static JNIEnv* cacheEnv(JavaVM* jvm);
    static JavaVM* _psJavaVM;
};


class JString2String
{
public:
	JString2String(jstring str){
		if(str){
			JNIEnv* env = JniHelper::GetEnv();
			jboolean isCopy = JNI_FALSE;
			const char* utfchar = env->GetStringUTFChars(str,&isCopy);
			string = utfchar;
			if (isCopy == JNI_TRUE) {
				env->ReleaseStringUTFChars(str,utfchar);
			}
			env->DeleteLocalRef(str);
		}
	};
	std::string getString(){
		return string;
	}
	const char* c_str(){
		return string.c_str();
	}
private:
	std::string string;
};


}
#endif
