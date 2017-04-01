//
//  V8Context.cpp
//  samples
//
//  Created by 廖敏 on 16/9/8.
//
//

#include "V8Context.hpp"
#include "JniHelper.h"
#include "Config.h"

namespace FQ {
    
    static vector<V8Context*> contexts;
    static V8ScriptEngine* engine;
    static jlong index = 0;
    //java long max_value
    static jlong MAX_INDEX = 9223372036854775807;


    RefInfo::RefInfo(Local<Value> value, jlong v8Id, jlong javaId):
            persistent(Isolate::GetCurrent(),value)
    {
        this->javaId = javaId;
        this->v8Id = v8Id;
        if(javaId != 0){
            persistent.SetWeak(this,[](const WeakCallbackInfo<RefInfo>& data){
                RefInfo* parameter = data.GetParameter();
                JavaMethodInfo methodInfo;
                if (JniHelper::GetJavaStaticMethodInfo(methodInfo,"com/liam/library/v8/JavaReference","releaseJavaRef","(I)V")){
                    methodInfo.callStaticVoidMethod(1,parameter->javaId);
                }
                parameter->persistent.ClearWeak();
                parameter->persistent.Reset();
                delete parameter;
            },WeakCallbackType::kParameter);
        }

    }

    RefInfo::~RefInfo() {

    }

    void RefInfo::Clear() {
        if(!persistent.IsWeak()){
            persistent.Reset();
            delete this;
        }
    }

    void RefInfo::Exist() {
        if(persistent.IsWeak())persistent.ClearWeak();
        persistent.Reset();
        delete this;
    }

    Local<Value> RefInfo::Get() {
        Isolate* isolate = Isolate::GetCurrent();
        if(!persistent.IsEmpty()){
            return persistent.Get(isolate);
        }
        return v8::Undefined(isolate);
    }





    V8Context::V8Context(){
        if(!engine){
            engine = new V8ScriptEngine("");
        }
        isolate = Isolate::New(*engine->create_params);
        m_isolate_scope = new Isolate::Scope(isolate);
        contexts.push_back(this);
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope handle_scope(isolate);
        // Create a new context.
        Local<Context> context = Context::New(isolate);
        context->SetAlignedPointerInEmbedderData(1,reinterpret_cast<void*>(this));
        context->Enter();
        m_global_ocontext = new Global<Context>(isolate,context);
        this->init(context);
    }
    
    void V8Context::init(Local<Context> context){

    }

    V8Context::~V8Context(){
        {
            HandleScope scope(Isolate::GetCurrent());
            Local<Context> context =  m_global_ocontext->Get(Isolate::GetCurrent());
            context->Exit();
        }
        m_global_ocontext->Reset();
        delete m_global_ocontext;
        for(vector<V8Context*>::iterator it  = contexts.begin(); it != contexts.end(); )
        {
            V8Context* v8Context = *it;
            if(v8Context == this){
                contexts.erase(it);
            }else{
                it++;
            }
        }
        unordered_map<jlong, RefInfo*>::iterator it  = persistents.begin();
        while (it != persistents.end()) {
            RefInfo* p = it->second;
            p->Exist();
            persistents.erase(it);
            it  = persistents.begin();
        }

        delete m_isolate_scope;
        isolate->Dispose();
        if(contexts.empty()){
//        	DELETE_POINTER(engine);
        }
    }
    
    
    Local<Value> V8Context::executeString(const char *source,bool& hasException){
        Isolate *isolate = Isolate::GetCurrent();
        EscapableHandleScope handle_scope(isolate);
        Local<Value> result = v8::Undefined(isolate);
        Local<Context> context = m_global_ocontext->Get(isolate);
        Context::Scope context_scope(context);
        if(source){
            Local<String> jsSource = NewV8String(source);
            v8::TryCatch try_catch(isolate);
            try_catch.SetVerbose(true);
            // Compile the source code.
            MaybeLocal<Script> maybeScript = Script::Compile(context, jsSource);
            if(!maybeScript.IsEmpty()){
                Local<Script> script = maybeScript.ToLocalChecked();
                MaybeLocal<Value> res = script->Run(context);
                if (!res.IsEmpty()) {
                    result = res.ToLocalChecked();
//                    string s =this->toString(result);
//                    LOGE("%s",s.c_str());
                }
            }
            Local<Value> exception = try_catch.Exception();
            if(!exception.IsEmpty()){
                if(exception->IsObject()){
                    Local<Object> exc = Local<Object>::Cast(exception);
                    Local<Value> stack = exc->Get(context, NewV8String("stack")).FromMaybe(exception);
                    result = stack;
//                    LOGE("Exception: %s\n", toString(stack).c_str());
                }else{
                    String::Utf8Value exception_str(exception);
//                    LOGE("Exception: %s\n", *exception_str);
                    result = exception;
                }
                hasException = true;

            }
        }
        return handle_scope.Escape(result);
    }
    
    V8Context* V8Context::GetCurrentContext(){
        Isolate* isolate = Isolate::GetCurrent();
        Local<Context> context = isolate->GetCurrentContext();
        for(vector<V8Context*>::iterator it  = contexts.begin(); it != contexts.end(); )
        {
            V8Context* v8Context = *it;
            Local<Context> cContext = v8Context->m_global_ocontext->Get(isolate);
            if(cContext == context){
                return v8Context;
            }
            it++;
        }
        return nullptr;
    }
    
    Local<Object> V8Context::GetGlobalObject(){
        Local<Context> context =  m_global_ocontext->Get(Isolate::GetCurrent());
        return context->Global();
    }
    
    
    Local<Context> V8Context::getV8Context(){
        return m_global_ocontext->Get(Isolate::GetCurrent());
    }

    string V8Context::toString(Local<v8::Value> object){
    	if(object->IsString()||object->IsRegExp()||object->IsFunction()){
			String::Utf8Value utf8(object);
			return string(*utf8);
		}
        v8::Local<v8::Context> context = m_global_ocontext->Get(Isolate::GetCurrent());
        if(object->IsObject()){
            MaybeLocal<String> str = JSON::Stringify(context,object->ToObject());
            if(str.IsEmpty()){
                return  "<string conversion failed>";
            }
            Local<String> s = str.ToLocalChecked();
            v8::String::Utf8Value str2(s);
            return string(*str2?*str2:"<string conversion failed>");
        }

        Local<Object> global = context->Global();
        Local<Object> JSON = global->Get(context,NewV8String("JSON")).ToLocalChecked()->ToObject(context).ToLocalChecked();
        Local<Value> argv[] = { object };
        Local<Function> JSON_stringify = Local<Function>::Cast(JSON->Get(context,NewV8String("stringify")).ToLocalChecked());
        v8::String::Utf8Value str(JSON_stringify->Call(context,JSON, 1, argv).ToLocalChecked());
        return string(*str?*str:"<string conversion failed>");
    }
    
    
    jlong V8Context::toPersistent(Local<Value> value,jlong javaRef){
        jlong identity = 0;
        while (true){
            identity = ++index;
            if(identity == MAX_INDEX){
                index = 1;
                identity = index;
            }
            if(!persistents[identity]) break;
        }
        persistents[identity] =  new RefInfo(value,identity,javaRef);// new Persistent<Value>(Isolate::GetCurrent(),value);
//        persistents[identity]->MarkIndependent();
//        Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(999999999999999);
//        Isolate::GetCurrent()->RequestGarbageCollectionForTesting(v8::Isolate::kFullGarbageCollection);
        return identity;
    }
    
    void V8Context::toWeak(jlong identity){
        unordered_map<jlong, RefInfo*>::iterator it = persistents.find(identity);
        if(it != persistents.end()){
            RefInfo* handle = it->second;
            persistents.erase(it);
            handle->Clear();
        }
//        LOGE("toweak %d-%d",persistents.size(),persistents_cache.size());
    }
    
    Local<Value> V8Context::toLocal(jlong identity){
        Isolate* isolate = Isolate::GetCurrent();
        unordered_map<jlong, RefInfo*>::iterator it = persistents.find(identity);
        if(it != persistents.end()){
            RefInfo* handle = it->second;
            return handle->Get();
        }
        return v8::Undefined(isolate);
    }

}
