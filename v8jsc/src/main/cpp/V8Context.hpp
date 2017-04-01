//
//  V8Context.hpp
//  samples
//
//  Created by 廖敏 on 16/9/8.
//
//

#ifndef V8Context_hpp
#define V8Context_hpp

#include <stdio.h>
#include "V8ScriptEngine.hpp"
#include <unordered_map>

using namespace v8;
using namespace std;

#ifndef jlong
typedef long long jlong;
#endif

namespace FQ {

    class RefInfo{
        public:
            RefInfo(Local<Value> value,jlong v8Id,jlong javaId);
            Local<Value> Get();
            void Clear();
            void Exist();
        private:
             ~RefInfo();
            Persistent<Value> persistent;
            jlong v8Id ;
            jlong javaId;

    };


    class V8Context {
        public:
            static V8Context* GetCurrentContext();
            Local<Object> GetGlobalObject();
            Local<Context> getV8Context();
            V8Context();
            virtual ~V8Context();
            Local<Value> executeString(const char* source,bool& hasException);
            std::string toString(Local<Value> object);
            jlong toPersistent(Local<Value> value,jlong javaRef = 0);
            void toWeak(jlong address);
            Local<Value> toLocal(jlong identity);
        protected:
            virtual void init(Local<Context> context);
        private:
            v8::Isolate::Scope* m_isolate_scope; //keep isolate scope not exit;
            Isolate* isolate;
            Global<Context> *m_global_ocontext;
            unordered_map<jlong, RefInfo*> persistents;
        };

    }

#endif /* V8Context_hpp */
