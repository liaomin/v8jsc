/*
 * JniHelper.cpp
 *
 *  Created on: 2014年8月28日
 *      Author: liaomin
 */

#include "JniHelper.h"
#include <android/log.h>
#include <string.h>
#include <pthread.h>
#include "Config.h"
#include "V8ScriptEngine.hpp"

#define JAVA_OBJECT_MANAGER_CLASS "com/fq/liam/jni/ObjectManager"

extern "C" {

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	FQ::JniHelper::SetJavaVM(vm);
	return JNI_VERSION_1_4;
}

}

namespace FQ {
static pthread_key_t g_key;

JavaVM* JniHelper::_psJavaVM = nullptr;

JavaVM* JniHelper::GetJavaVM() {
	pthread_t thisthread = pthread_self();
	LOGD("JniHelper::getJavaVM(), pthread_self() = %ld", thisthread);
	return _psJavaVM;
}

void JniHelper::SetJavaVM(JavaVM *javaVM) {
	pthread_t thisthread = pthread_self();
	LOGD("JniHelper::setJavaVM(%p), pthread_self() = %ld", javaVM, thisthread);
	_psJavaVM = javaVM;
	pthread_key_create(&g_key, nullptr);
}

JNIEnv* JniHelper::cacheEnv(JavaVM* jvm) {
	JNIEnv* _env = nullptr;
	jint ret = jvm->GetEnv((void**) &_env, JNI_VERSION_1_4);

	switch (ret) {
	case JNI_OK:
		// Success!
		pthread_setspecific(g_key, _env);
		return _env;

	case JNI_EDETACHED:
		if (jvm->AttachCurrentThread(&_env, nullptr) < 0) {
			LOGE("Failed to get the environment using AttachCurrentThread()");

			return nullptr;
		} else {
			pthread_setspecific(g_key, _env);
			return _env;
		}

	case JNI_EVERSION:
		LOGE("JNI interface version 1.4 not supported");
	default:
		LOGE("Failed to get the environment using GetEnv()");
		return nullptr;
	}
}

JNIEnv* JniHelper::GetEnv() {
	JNIEnv *_env = (JNIEnv *) pthread_getspecific(g_key);
	if (_env == nullptr)
		_env = JniHelper::cacheEnv(_psJavaVM);
	return _env;
}

bool JniHelper::GetJavaMethodInfo(JavaMethodInfo &methodinfo, const char *className, const char *methodName, const char *parameterTypes) {
	JNIEnv* env = GetEnv();
	methodinfo.clear();
	do {
		jclass classID = env->FindClass(className);
		if (!classID) {
			LOGE("Failed to find class %s", className);
			env->ExceptionClear();
			break;
		}
		jmethodID methodID = env->GetMethodID(classID, methodName, parameterTypes);
		if (!methodID) {
			LOGE("Failed to find method %s <%s> for class %s,", methodName,parameterTypes,className);
			env->ExceptionClear();
			break;
		}
		methodinfo.classID = classID;
		methodinfo.env = env;
		methodinfo.methodID = methodID;
		return true;
	} while (false);
	return false;
}

bool JniHelper::GetJavaMethodInfo(JavaMethodInfo &methodinfo,jobject object,const char *methodName,const char *parameterTypes)
{
	JNIEnv* env = GetEnv();
	methodinfo.clear();
	do {
		if(!object){
			LOGE("jobjec is null");
			break;
		}
		jclass classID = env->GetObjectClass(object);
		jmethodID methodID = env->GetMethodID(classID, methodName, parameterTypes);
		if (!methodID) {
			LOGE("Failed to find method %s <%s> ", methodName,parameterTypes);
			env->ExceptionClear();
			break;
		}
		methodinfo.classID = classID;
		methodinfo.env = env;
		methodinfo.methodID = methodID;
		return true;
	} while (false);
	return false;
}

bool JniHelper::GetJavaStaticMethodInfo(JavaMethodInfo &methodinfo, const char *className, const char *methodName, const char *parameterTypes) {
	JNIEnv* env = GetEnv();
	methodinfo.clear();
	do {
		jclass classID = env->FindClass(className);
		if (!classID) {
			LOGE("Failed to find class %s", className);
			env->ExceptionClear();
			break;
		}
		jmethodID methodID = env->GetStaticMethodID(classID, methodName, parameterTypes);
		if (!methodID) {
			LOGE("Failed to find static method %s for class %s ,parameterTypes :%s", methodName, className,parameterTypes);
			env->ExceptionClear();
			break;
		}
		methodinfo.classID = classID;
		methodinfo.env = env;
		methodinfo.methodID = methodID;
		return true;
	} while (false);
	return false;
}

jobject JniHelper::NewJaveObject(const char* className, const char *parameterTypes, ...) {
	va_list args;
	va_start(args, parameterTypes);
	jobject javaInstance = JniHelper::NewJaveObjectV(className, parameterTypes,args);
	va_end(args);
	return javaInstance;
}

jobject JniHelper::NewJaveObjectA(const char *className,const char *parameterTypes,jvalue *args){
	JNIEnv* env = GetEnv();
	JavaMethodInfo info;
	if (JniHelper::GetJavaMethodInfo(info, className, "<init>", parameterTypes)) {
		jobject javaInstance = info.env->NewObjectA(info.classID, info.methodID,args);
		return javaInstance;
	}
	return nullptr;
}

jobject JniHelper::NewJaveObjectV(const char *className,const char *parameterTypes,va_list args){
	JNIEnv* env = GetEnv();
	JavaMethodInfo info;
	if (JniHelper::GetJavaMethodInfo(info, className, "<init>", parameterTypes)) {
		jobject javaInstance = info.env->NewObjectV(info.classID, info.methodID,args);
		return javaInstance;
	}
	return nullptr;
}

void JniHelper::CacheJavaObject(FQ::JavaObject *cobject,jobject obj){
	JavaMethodInfo methodInfo;
	if(GetJavaStaticMethodInfo(methodInfo,JAVA_OBJECT_MANAGER_CLASS,"CacheObject","(JLjava/lang/Object;)V")){
		methodInfo.callStaticVoidMethod(2,(jlong)cobject,obj);
	}
}

void JniHelper::RemoveCache(FQ::JavaObject *cobject){
	JavaMethodInfo methodInfo;
	if (GetJavaStaticMethodInfo(methodInfo,JAVA_OBJECT_MANAGER_CLASS,"RemoveCache","(J)V")){
		methodInfo.callStaticVoidMethod(1,(jlong)cobject);
	}
}

jobject JniHelper::GetJavaObject(FQ::JavaObject *cobject)
{
	JavaMethodInfo methodInfo;
	if (GetJavaStaticMethodInfo(methodInfo,JAVA_OBJECT_MANAGER_CLASS,"GetObjectById","(J)Ljava/lang/Object;")){
		return methodInfo.callStaticObjectMethod(1,(jlong)cobject);
	}
	return nullptr;
}
}
