/*
 * JavaMethodInfo.h
 *
 *  Created on: 2015年9月18日
 *      Author: liaomin
 */

#ifndef JAVAMETHODINFO_H_
#define JAVAMETHODINFO_H_

#include <jni.h>

#ifndef __NDK_FPABI__
//#define __NDK_FPABI__ __attribute__((pcs("aapcs")))
#define __NDK_FPABI__
#endif

namespace FQ{

class JavaMethodInfo {
public:
    JNIEnv *    env;
    jclass      classID;
    jmethodID   methodID;
    JavaMethodInfo(){
    	env = nullptr;
    	classID = nullptr;
    	methodID = nullptr;
    }
    virtual ~JavaMethodInfo(){
    	this->clear();
    }

    void clear(){
    	if(env && classID){
			env->DeleteLocalRef(classID);
		}
    }

    void callVoidMethod(jobject obj,...){
       	va_list args;
       	va_start(args, obj);
       	callVoidMethodV(obj,args);
       	va_end(args);
    }

    void callVoidMethodA(jobject obj,jvalue* args){
    	if(env && classID && methodID){
		  env->CallVoidMethodA(obj,methodID,args);
		}
	}

    void callVoidMethodV(jobject obj,va_list args){
    	if(env && classID && methodID){
		  env->CallVoidMethodV(obj,methodID,args);
		}
	}

#define CALL_JTYPE_METHODV(_jtype, _jname)           		 	\
    __NDK_FPABI__  												\
    _jtype call##_jname##MethodV(jobject obj,va_list args)      	\
    {  															\
    	_jtype res = 0;											\
    	if(env && classID && methodID){ 						\
    		res = env->Call##_jname##MethodV(obj,methodID,args);\
    	}														\
    	return res; 											\
	}

    CALL_JTYPE_METHODV(jobject, Object)
	CALL_JTYPE_METHODV(jboolean, Boolean)
	CALL_JTYPE_METHODV(jbyte, Byte)
	CALL_JTYPE_METHODV(jchar, Char)
	CALL_JTYPE_METHODV(jshort, Short)
	CALL_JTYPE_METHODV(jint, Int)
	CALL_JTYPE_METHODV(jlong, Long)
	CALL_JTYPE_METHODV(jfloat, Float)
	CALL_JTYPE_METHODV(jdouble, Double)



#define CALL_JTYPE_METHOD(_jtype, _jname)           		 	\
    __NDK_FPABI__  												\
    _jtype call##_jname##Method(jobject obj, ...)      	 		\
    {  															\
    	va_list args;			 								\
    	va_start(args, obj); 									\
    	_jtype res = 0;											\
    	res =  call##_jname##MethodV(obj,args);					\
    	va_end(args); 											\
    	return res; 											\
	}

    CALL_JTYPE_METHOD(jobject, Object)
    CALL_JTYPE_METHOD(jboolean, Boolean)
    CALL_JTYPE_METHOD(jbyte, Byte)
    CALL_JTYPE_METHOD(jchar, Char)
    CALL_JTYPE_METHOD(jshort, Short)
    CALL_JTYPE_METHOD(jint, Int)
    CALL_JTYPE_METHOD(jlong, Long)
    CALL_JTYPE_METHOD(jfloat, Float)
    CALL_JTYPE_METHOD(jdouble, Double)


#define CALL_JTYPE_METHODA(_jtype, _jname)           		 	\
    __NDK_FPABI__  												\
    _jtype call##_jname##MethodA(jobject obj,jvalue* args)      	\
    {  															\
    	_jtype res = 0;											\
    	if(env && classID && methodID){ 						\
    		res = env->Call##_jname##MethodA(obj,methodID,args);\
    	}														\
    	return res; 											\
	}

    CALL_JTYPE_METHODA(jobject, Object)
    CALL_JTYPE_METHODA(jboolean, Boolean)
    CALL_JTYPE_METHODA(jbyte, Byte)
    CALL_JTYPE_METHODA(jchar, Char)
    CALL_JTYPE_METHODA(jshort, Short)
    CALL_JTYPE_METHODA(jint, Int)
    CALL_JTYPE_METHODA(jlong, Long)
    CALL_JTYPE_METHODA(jfloat, Float)
    CALL_JTYPE_METHODA(jdouble, Double)

    void callStaticVoidMethod(int argc,...){
    	va_list args;
    	va_start(args, argc);
    	if(env && classID && methodID){
			env->CallStaticVoidMethodV(classID,methodID,args);
		}
    	va_end(args);
    }

    void callStaticVoidMethodV(va_list args){
		if(env && classID && methodID){
			env->CallStaticVoidMethodV(classID,methodID,args);
		}
	}

    void callStaticVoidMethodA(jvalue *args){
   		if(env && classID && methodID){
   			env->CallStaticVoidMethodA(classID,methodID,args);
   		}
   	}


#define CALL_JTYPE_STATIC_METHODV(_jtype, _jname)           	\
    __NDK_FPABI__  												\
    _jtype callStatic##_jname##MethodV(va_list args) \
    {  															\
    	_jtype res = 0;											\
    	if(env && classID && methodID){ 						\
    		res = env->CallStatic##_jname##MethodV(classID,methodID,args);\
    	}														\
    	return res; 											\
	}

    CALL_JTYPE_STATIC_METHODV(jobject, Object)
    CALL_JTYPE_STATIC_METHODV(jboolean, Boolean)
    CALL_JTYPE_STATIC_METHODV(jbyte, Byte)
    CALL_JTYPE_STATIC_METHODV(jchar, Char)
    CALL_JTYPE_STATIC_METHODV(jshort, Short)
    CALL_JTYPE_STATIC_METHODV(jint, Int)
    CALL_JTYPE_STATIC_METHODV(jlong, Long)
    CALL_JTYPE_STATIC_METHODV(jfloat, Float)
    CALL_JTYPE_STATIC_METHODV(jdouble, Double)

#define CALL_JTYPE_STATIC_METHODA(_jtype, _jname)           	\
    __NDK_FPABI__  												\
    _jtype callStatic##_jname##MethodA(jvalue *args) \
    {  															\
    	_jtype res = 0;											\
    	if(env && classID && methodID){ 						\
    		res = env->CallStatic##_jname##MethodA(classID,methodID,args);\
    	}														\
    	return res; 											\
	}

    CALL_JTYPE_STATIC_METHODA(jobject, Object)
    CALL_JTYPE_STATIC_METHODA(jboolean, Boolean)
    CALL_JTYPE_STATIC_METHODA(jbyte, Byte)
    CALL_JTYPE_STATIC_METHODA(jchar, Char)
    CALL_JTYPE_STATIC_METHODA(jshort, Short)
    CALL_JTYPE_STATIC_METHODA(jint, Int)
    CALL_JTYPE_STATIC_METHODA(jlong, Long)
    CALL_JTYPE_STATIC_METHODA(jfloat, Float)
    CALL_JTYPE_STATIC_METHODA(jdouble, Double)


#define CALL_JTYPE_STATIC_METHOD(_jtype, _jname)           		\
    __NDK_FPABI__  												\
    _jtype callStatic##_jname##Method(int argc, ...)      	 	\
    {  															\
    	va_list args;			 								\
    	va_start(args, argc); 									\
    	_jtype res = 0;											\
    	res = callStatic##_jname##MethodV(args);    				\
    	va_end(args); 											\
    	return res; 											\
	}

    CALL_JTYPE_STATIC_METHOD(jobject, Object)
    CALL_JTYPE_STATIC_METHOD(jboolean, Boolean)
    CALL_JTYPE_STATIC_METHOD(jbyte, Byte)
	CALL_JTYPE_STATIC_METHOD(jchar, Char)
	CALL_JTYPE_STATIC_METHOD(jshort, Short)
	CALL_JTYPE_STATIC_METHOD(jint, Int)
	CALL_JTYPE_STATIC_METHOD(jlong, Long)
	CALL_JTYPE_STATIC_METHOD(jfloat, Float)
	CALL_JTYPE_STATIC_METHOD(jdouble, Double)

};


}

#endif /* JAVAMETHODINFO_H_ */
