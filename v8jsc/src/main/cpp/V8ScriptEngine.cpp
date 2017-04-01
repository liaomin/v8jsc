//
//  V8ScriptEngine.cpp
//
//  Created by 廖敏 on 15/12/26.
//
//

#include "V8ScriptEngine.hpp"
#include "Config.h"

namespace FQ {
    V8ScriptEngine::V8ScriptEngine(const char *defaultPath) {
//    const char* flags = "--stack-trace-limit 10";
//    const char* flags = "--expose_gc";
//    v8::V8::SetFlagsFromString(flags, (int)strlen(flags));
//    V8::InitializeICUDefaultLocation(defaultPath);
//    V8::InitializeExternalStartupData(defaultPath);
        V8::InitializeICUDefaultLocation(defaultPath);
        V8::InitializeExternalStartupData(defaultPath);
        platform = platform::CreateDefaultPlatform();
        V8::InitializePlatform(platform);
        V8::Initialize();

        // Create a new Isolate and make it the current one.
        create_params = new Isolate::CreateParams();
        create_params->array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    }


    V8ScriptEngine::~V8ScriptEngine() {
        V8::Dispose();
        V8::ShutdownPlatform();
        delete platform;
        delete create_params->array_buffer_allocator;
        delete create_params;
    }


}


