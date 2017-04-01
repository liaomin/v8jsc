#include <jni.h>
#include "JniHelper.h"
#include "Config.h"
#include "V8Context.hpp"

extern "C" {

using namespace v8;
using namespace FQ;

#define JAVA_OBJECT_ID_NAME "__java_object_id__"
#define JAVA_CONTEXT_ID_NAME "__java_context_id__"
#define JSTYPE_NULL 		0
#define JSTYPE_STRING 		1
#define JSTYPE_NUMBER 		2
#define JSTYPE_ARRAY 		3
#define JSTYPE_DATE 		4
#define JSTYPE_JAVAOBJECT 	5
#define JSTYPE_BOOLEAN 		6
#define JSTYPE_JSOBJECT  	7
#define JSTYPE_JSFUNCTION 	8

bool valueIsJavaObject(Local<Value>& value){
	if(value->IsObject()){
		Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
		Local<Object> obj = value->ToObject(v8_context).ToLocalChecked();
		MaybeLocal<Value> objId = obj->Get(v8_context,NewV8String(JAVA_OBJECT_ID_NAME));
		return !objId.IsEmpty() && objId.ToLocalChecked()->IsNumber();
	}
	return false;
}

#define V8_NATIVE_METHOD(returnType,className,methodName) \
		JNIEXPORT returnType JNICALL Java_com_liam_library_v8_##className##_##methodName

V8_NATIVE_METHOD(jint,JSConver,nativeGetJSType)(JNIEnv* env,jobject thiz,jlong contextPrt,jlong valuePtr){
	V8Context* context = reinterpret_cast<V8Context*>(contextPrt);
	if(context && valuePtr){
		Local<Value> value = context->toLocal(valuePtr);
		if(value->IsNumber()){
			return JSTYPE_NUMBER;
		}
		if(value->IsString()){
			return JSTYPE_STRING;
		}
		if(value->IsBoolean()){
			return JSTYPE_BOOLEAN;
		}
		if(value->IsDate()){
			return JSTYPE_DATE;
		}
		if(value->IsArray()){
			return JSTYPE_ARRAY;
		}
		if(value->IsFunction()){
			return JSTYPE_JSFUNCTION;
		}
		if(value->IsObject()){
			if(valueIsJavaObject(value)){
				return JSTYPE_JAVAOBJECT;
			}
			return JSTYPE_JSOBJECT;
		}
	}
	return JSTYPE_NULL;
}

V8_NATIVE_METHOD(jlongArray,JSConver,nativeGetArrayValues)(JNIEnv* env,jobject thiz,jlong contextPrt,jlong valuePtr){
	V8Context* context = reinterpret_cast<V8Context*>(contextPrt);
	if(context && valuePtr){
		Local<Value> value = context->toLocal(valuePtr);
		if(value->IsArray()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Array> jsArray = Local<Array>::Cast(value);
			uint32_t length = jsArray->Length();
			jlongArray array  = env->NewLongArray(length);
			jlong* buffer = new jlong[length];
			for (uint32_t i = 0; i < length; i++) {
				Local<Value> argi = jsArray->Get(v8_context,i).ToLocalChecked();
				jlong id = context->toPersistent(argi);
				buffer[i] = id;
			}
			env->SetLongArrayRegion(array,0,length,buffer);
			delete[] buffer;
			return array;
		}
	}
	return NULL;
}

V8_NATIVE_METHOD(jlong,JSConver,nativeGetObjectKeys)(JNIEnv* env,jobject thiz,jlong contextPrt,jlong valuePtr){
	V8Context* context = reinterpret_cast<V8Context*>(contextPrt);
	if(context && valuePtr){
		Local<Value> value = context->toLocal(valuePtr);
		if(value->IsObject()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Object> jsObject = value->ToObject(v8_context).ToLocalChecked();
			Local<Array> namse = jsObject->GetPropertyNames(v8_context).ToLocalChecked();
			jlong id = context->toPersistent(namse);
			return id;
		}
	}
	return 0;
}

V8_NATIVE_METHOD(jlong,JSContext,nativeInit)(JNIEnv* env,jobject thiz){
	V8Context* context = new V8Context();
	HandleScope handle_scope(Isolate::GetCurrent());
	JavaMethodInfo method;
	if(JniHelper::GetJavaMethodInfo(method,thiz,"setGlobalObject","(J)V")){
		Local<Object> c = context->GetGlobalObject();
		method.callVoidMethod(thiz, context->toPersistent(c));
	}
	return reinterpret_cast<jlong>(context);
}

V8_NATIVE_METHOD(void,JSContext,nativeRelease)(JNIEnv* env,jobject thiz,jlong address){
	V8Context* context = reinterpret_cast<V8Context*>(address);
	if(context){
		delete context;
	}
}

V8_NATIVE_METHOD(void,JSContext,nativeHandleScope)(JNIEnv* env,jobject thiz,jlong ptr,jobject block){
	V8Context* context = reinterpret_cast<V8Context*>(ptr);
	if(context && block){
		HandleScope handle_scope(Isolate::GetCurrent());
		JavaMethodInfo method;
		if(JniHelper::GetJavaMethodInfo(method,block,"enterHandleScope","()V")){
			method.callVoidMethod(block);
		}
	}
}

V8_NATIVE_METHOD(jlong,JSContext,nativeEvaluateScript)(JNIEnv* env,jobject thiz,jlong ptr,jstring script){
	V8Context* context = reinterpret_cast<V8Context*>(ptr);
	if(context){
		JString2String src(script);
		bool hasException = false;
		Local<Value> result = context->executeString(src.c_str(),hasException);
		jlong id = context->toPersistent(result);
		if(hasException){
			JavaMethodInfo method;
			if(JniHelper::GetJavaMethodInfo(method,thiz,"setException","(J)V")){
				method.callVoidMethod(thiz,id);
			}
		}
		return id;
	}
	return 0;
}


V8_NATIVE_METHOD(void,JSValue,nativeRelease)(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context && ptr){
		context->toWeak(ptr);
	}
}

V8_NATIVE_METHOD(jlong,JSValue,nativeGet)(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr,jstring key){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	Local<Value> result = v8::Undefined(Isolate::GetCurrent());
	if(context && ptr && key){
		Local<Value> value = context->toLocal(ptr);
		if(value->IsObject()){
			 Local<Context> con = Isolate::GetCurrent()->GetCurrentContext();
			 Local<Object> obj = value->ToObject(con).ToLocalChecked();
			 JString2String src(key);
			 result =  obj->Get(con,NewV8String(src.c_str())).FromMaybe(result);
		 }
	}
	jlong id = context->toPersistent(result);
	return id;
}

//check is
#define V8_JSVALUE_CHECK_METHOD(methodName,returnCode)  \
		JNIEXPORT jboolean JNICALL Java_com_liam_library_v8_JSValue_##methodName(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr){\
			V8Context* context = reinterpret_cast<V8Context*>(cPtr);\
			if(context && ptr){\
				Local<Value> value = context->toLocal(ptr); \
				returnCode; \
			} \
			return false; \
		}\


V8_JSVALUE_CHECK_METHOD(nativeIsUndefined,return value->IsUndefined());
V8_JSVALUE_CHECK_METHOD(nativeIsNull,return value->IsNull());
V8_JSVALUE_CHECK_METHOD(nativeIsBoolean,return value->IsBoolean());
V8_JSVALUE_CHECK_METHOD(nativeIsNumber,return value->IsNumber());
V8_JSVALUE_CHECK_METHOD(nativeIsString,return value->IsString());
V8_JSVALUE_CHECK_METHOD(nativeIsObject,return value->IsObject());
V8_JSVALUE_CHECK_METHOD(nativeIsJavaObject,return valueIsJavaObject(value));
V8_JSVALUE_CHECK_METHOD(nativeIsArray,return value->IsArray());
V8_JSVALUE_CHECK_METHOD(nativeIsDate,return value->IsDate());



//conver to
#define V8_JSVALUE_TO_METHOD(returntype,methodName,returnCode,defaultReturn)  \
		JNIEXPORT returntype JNICALL Java_com_liam_library_v8_JSValue_##methodName(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr){\
			V8Context* context = reinterpret_cast<V8Context*>(cPtr);\
			if(context && ptr){\
				Local<Value> value = context->toLocal(ptr); \
				returnCode; \
			} \
			defaultReturn; \
		}\

V8_JSVALUE_TO_METHOD(jstring,nativeToString,{
		std::string str = context->toString(value);
		return env->NewStringUTF(str.c_str());
},return NULL)
V8_JSVALUE_TO_METHOD(jobject,nativeToObject,{
		if(value->IsObject()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Object> obj = value->ToObject(v8_context).ToLocalChecked();
			MaybeLocal<Value> objId = obj->Get(v8_context,NewV8String(JAVA_OBJECT_ID_NAME));
			if(!objId.IsEmpty()){
				Local<Value> id = objId.ToLocalChecked();
				if(id->IsNumber()){
					long objectId = id->ToInteger(v8_context).ToLocalChecked()->Value();
					JavaMethodInfo methodInfo;
					if (JniHelper::GetJavaStaticMethodInfo(methodInfo,"com/liam/library/v8/JavaReference","getObject","(I)Ljava/lang/Object;")){
						return methodInfo.callStaticObjectMethod(1,objectId);
					}
				}
			}
		}
},return NULL)
V8_JSVALUE_TO_METHOD(jboolean,nativeToBool,{
	if(value->IsBoolean()){
		return value->ToBoolean(Isolate::GetCurrent()->GetCurrentContext()).ToLocalChecked()->Value();
	}
},return false)
V8_JSVALUE_TO_METHOD(jdouble,nativeToDouble,{
	if(value->IsNumber()){
		return value->ToNumber(Isolate::GetCurrent()->GetCurrentContext()).ToLocalChecked()->Value();
	}
},return 0)
V8_JSVALUE_TO_METHOD(jint,nativeToInt,{
	if(value->IsNumber()){
		return value->ToInteger(Isolate::GetCurrent()->GetCurrentContext()).ToLocalChecked()->Value();
	}
},return 0)
V8_JSVALUE_TO_METHOD(jlong,nativeToDate,{
	if(value->IsDate()){
		return Local<Date>::Cast(value)->ValueOf();
	}
},return 0)


//set
#define V8_JSVALUE_SET_METHOD(methodName,valueType,setCode)  \
		JNIEXPORT void JNICALL Java_com_liam_library_v8_JSValue_##methodName(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr,jstring key,valueType value){\
			V8Context* context = reinterpret_cast<V8Context*>(cPtr);\
			if(context && ptr && key){\
				Local<Value> l_value = context->toLocal(ptr); \
				if(l_value->IsObject()){ \
					Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();\
					Local<Object> obj = l_value->ToObject(v8_context).ToLocalChecked();	\
					setCode \
				}\
			} \
		}\

V8_JSVALUE_SET_METHOD(nativeSetInt,jint,{
		JString2String jKey(key);
		obj->Set(v8_context,NewV8String(jKey.c_str()),Integer::New(Isolate::GetCurrent(),value));
})
V8_JSVALUE_SET_METHOD(nativeSetDouble,jdouble,{
		JString2String jKey(key);
		obj->Set(v8_context,NewV8String(jKey.c_str()),Number::New(Isolate::GetCurrent(),value));
})
V8_JSVALUE_SET_METHOD(nativeSetString,jstring,{
		if(value){
			JString2String jKey(key);
			JString2String jValue(value);
			obj->Set(v8_context,NewV8String(jKey.c_str()),NewV8String(jValue.c_str()));
		}
})
V8_JSVALUE_SET_METHOD(nativeSetBoolean,jboolean,{
		JString2String jKey(key);
		obj->Set(v8_context,NewV8String(jKey.c_str()),Boolean::New(Isolate::GetCurrent(),value));
})



V8_NATIVE_METHOD(void,JSValue,nativeSetJSValue)(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr,jstring key,jlong value){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	Local<Value> jsValue = context->toLocal(value);
	if(context && ptr && key ){
		Local<Value> l_value = context->toLocal(ptr);
		if(l_value->IsObject()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Object> obj = l_value->ToObject(v8_context).ToLocalChecked();
			JString2String jKey(key);
			obj->Set(v8_context,NewV8String(jKey.c_str()),jsValue);
		}
	}
}

V8_NATIVE_METHOD(void,JSValue,nativeSetDate)(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr,jstring key,jlong timestemp){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context && ptr && key){
		Local<Value> l_value = context->toLocal(ptr);
		if(l_value->IsObject()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Object> obj = l_value->ToObject(v8_context).ToLocalChecked();
			JString2String jKey(key);
			obj->Set(v8_context,NewV8String(jKey.c_str()),Date::New(v8_context,timestemp).ToLocalChecked());
		}
	}
}

//new
V8_NATIVE_METHOD(jlong,JSValue,nativeNewJaveObject)(JNIEnv* env,jobject thiz,jlong cPtr,jint jptr,jint jObject){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context && jObject){
		Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
		Local<Object> jObj = Object::New(Isolate::GetCurrent());
		jObj->Set(v8_context,NewV8String(JAVA_OBJECT_ID_NAME),Integer::New(Isolate::GetCurrent(),jObject));
		jObj->Set(v8_context,NewV8String(JAVA_CONTEXT_ID_NAME),Integer::New(Isolate::GetCurrent(),jptr));
		return context->toPersistent(jObj,jObject);
	}
	return 0;
}

V8_NATIVE_METHOD(jlong,JSValue,nativeNewBool)(JNIEnv* env,jobject thiz,jlong cPtr,jboolean value){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		Local<Boolean> jObj = Boolean::New(Isolate::GetCurrent(),value);
		return context->toPersistent(jObj);
	}
	return 0;
}

V8_NATIVE_METHOD(jlong,JSValue,nativeNewString)(JNIEnv* env,jobject thiz,jlong cPtr,jstring value){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		JString2String tValue(value);
		Local<Value> jObj = NewV8String(tValue.c_str());
		return context->toPersistent(jObj);
	}
	return 0;
}

V8_NATIVE_METHOD(jlong,JSValue,nativeNewDouble)(JNIEnv* env,jobject thiz,jlong cPtr,jdouble value){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		Local<Number> jObj = Number::New(Isolate::GetCurrent(),value);
		return context->toPersistent(jObj);
	}
	return 0;
}
V8_NATIVE_METHOD(jlong,JSValue,nativeNewInt)(JNIEnv* env,jobject thiz,jlong cPtr,jint value){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		Local<Integer> jObj = Integer::New(Isolate::GetCurrent(),value);
		return context->toPersistent(jObj);
	}
	return 0;
}
V8_NATIVE_METHOD(jlong,JSValue,nativeNewJSObject)(JNIEnv* env,jobject thiz,jlong cPtr){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		Local<Object> jObj = Object::New(Isolate::GetCurrent());
		return context->toPersistent(jObj);
	}
	return 0;
}
V8_NATIVE_METHOD(jlong,JSValue,nativeNewJSArray)(JNIEnv* env,jobject thiz,jlong cPtr){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		Local<Object> jObj = Array::New(Isolate::GetCurrent());
		return context->toPersistent(jObj);;
	}
	return 0;
}

V8_NATIVE_METHOD(jlong,JSValue,nativeNewRegex)(JNIEnv* env,jobject thiz,jlong cPtr,jstring pattern,jint flag){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context && pattern){
		JString2String tPattern(pattern);
		MaybeLocal<RegExp> jObj = RegExp::New(Isolate::GetCurrent()->GetCurrentContext(),NewV8String(tPattern.c_str()), static_cast<RegExp::Flags>(flag));
		if(!jObj.IsEmpty()){
			Local<RegExp> r = jObj.ToLocalChecked();
			return context->toPersistent(r);;
		}

	}
	return 0;
}
V8_NATIVE_METHOD(jlong,JSValue,nativeNewError)(JNIEnv* env,jobject thiz,jlong cPtr,jstring message){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		JString2String tMessage(message);
		Local<Value> jObj =  Exception::Error(NewV8String(tMessage.c_str()));
		return context->toPersistent(jObj);
	}
	return 0;
}
V8_NATIVE_METHOD(jlong,JSValue,nativeNewNull)(JNIEnv* env,jobject thiz,jlong cPtr){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		Local<Value> jObj = Null(Isolate::GetCurrent());
		return context->toPersistent(jObj);
	}
	return 0;
}

V8_NATIVE_METHOD(jlong,JSValue,nativeNewUndefined)(JNIEnv* env,jobject thiz,jlong cPtr){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context){
		Local<Value> jObj = Undefined(Isolate::GetCurrent());
		return context->toPersistent(jObj);
	}
	return 0;
}

V8_NATIVE_METHOD(jboolean,JSValue,nativeHasProperty)(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr,jstring prop){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context && ptr){
		Local<Value> value = context->toLocal(ptr);
		if(value->IsObject()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Object> obj = value->ToObject(v8_context).ToLocalChecked();
			JString2String src(prop);
			return obj->Has(NewV8String(src.c_str()));
		}
	}
	return false;
}

V8_NATIVE_METHOD(jlong,JSValue,nativeValueAtIndex)(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr,jint index){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context && ptr){
		Local<Value> value = context->toLocal(ptr);
		if(value->IsObject()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Object> obj = value->ToObject(v8_context).ToLocalChecked();
			Local<Value> o = obj->Get(Number::New(Isolate::GetCurrent(),index));
			return context->toPersistent(o);
		}
	}
	return 0;
}

V8_NATIVE_METHOD(jlong,JSValue,nativeCall)(JNIEnv* env,jobject thiz,jlong cPtr,jlong ptr,jboolean isConstruct,jlongArray args){
	V8Context* context = reinterpret_cast<V8Context*>(cPtr);
	if(context && ptr){
		Local<Value> value = context->toLocal(ptr);
		if(value->IsObject()){
			Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
			Local<Object> obj = value->ToObject(v8_context).ToLocalChecked();
			JNIEnv* env = JniHelper::GetEnv();
			int argLength = env->GetArrayLength(args);
			Local<Value> *argv = new Local<Value>[argLength];
			jlong buf[10];
			env->GetLongArrayRegion(args,0,argLength,buf);
			for(int i = 0; i < argLength ; i++){
				Local<Value> vi = context->toLocal(buf [i]);
				argv[i] = vi;
			}
			MaybeLocal<Value> res;
			if(isConstruct){
				res = obj->CallAsConstructor(v8_context,env->GetArrayLength(args),argv);
			}else{
				res = obj->CallAsFunction(v8_context,value,env->GetArrayLength(args),argv);
			}
			delete[] argv;
			if(!res.IsEmpty()){
				Local<Value> lRes = res.ToLocalChecked();
				return context->toPersistent(lRes);
			}
		}
	}
	return 0;
}


V8_NATIVE_METHOD(jlong,JSValue,nativeNewFunction)(JNIEnv* env,jobject thiz,jlong cPtr,jlong contextId,jlong jFunctionId){
	V8Context* contextv8 = reinterpret_cast<V8Context*>(cPtr);
	if(contextv8){
		Local<Context> v8_context = Isolate::GetCurrent()->GetCurrentContext();
		Local<Function> jsf = Function::New(v8_context,[](const FunctionCallbackInfo<Value>& args){
			Local<Context> context = Isolate::GetCurrent()->GetCurrentContext();
			MaybeLocal<Value> objId = args.Callee()->Get(context,NewV8String(JAVA_OBJECT_ID_NAME));
			MaybeLocal<Value> jContextId = args.Callee()->Get(context,NewV8String(JAVA_CONTEXT_ID_NAME));
			if(!objId.IsEmpty() && !jContextId.IsEmpty()){
				Local<Value> id = objId.ToLocalChecked();
				Local<Value> contextId = jContextId.ToLocalChecked();
				if(id->IsNumber() && contextId->IsNumber()){
					V8Context* v8context = reinterpret_cast<V8Context*>(context->GetAlignedPointerFromEmbedderData(1));
					if(v8context){
						int argLength = args.Length();
						JNIEnv* env = JniHelper::GetEnv();
						jlongArray array = env->NewLongArray(argLength);
						jlong* buffer = new jlong[argLength];
						for (int i = 0; i < args.Length(); i++) {
							Local<Value> argi = args[i];
							buffer[i] = v8context->toPersistent(argi);;
						}
						env->SetLongArrayRegion(array,0,argLength,buffer);
						delete[] buffer;
						Local<Value> c =  args.Callee();
						jlong jCallee = v8context->toPersistent(c);
						c =  args.This();
						jlong jThis = v8context->toPersistent(c);
						long objectId = id->ToInteger(context).ToLocalChecked()->Value();
						long jContextId = contextId->ToInteger(context).ToLocalChecked()->Value();
						JavaMethodInfo methodInfo;
						if (JniHelper::GetJavaStaticMethodInfo(methodInfo,"com/liam/library/v8/JavaReference","invokeJsFunction","(II[JJJ)J")){
							jlong  identity = methodInfo.callStaticLongMethod(2,jContextId,objectId,array,jCallee,jThis);
							Local<Value> result = v8context->toLocal(identity);
							args.GetReturnValue().Set(result);
							v8context->toWeak(identity);
						}
						env->DeleteLocalRef(array);
					}
				}
			}
		}).ToLocalChecked();
		jsf->Set(v8_context,NewV8String(JAVA_OBJECT_ID_NAME),Integer::New(Isolate::GetCurrent(),jFunctionId));
		jsf->Set(v8_context,NewV8String(JAVA_CONTEXT_ID_NAME),Integer::New(Isolate::GetCurrent(),contextId));
		return contextv8->toPersistent(jsf,jFunctionId);
	}
	return 0;
}

}
