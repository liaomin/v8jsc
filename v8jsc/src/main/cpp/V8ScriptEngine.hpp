//
//  V8ScriptEngine.hpp
//
//  Created by 廖敏 on 15/12/26.
//
//

#ifndef V8ScriptEngine_hpp
#define V8ScriptEngine_hpp

#include "v8.h"
#include <stdio.h>
#include <string>
#include "libplatform/libplatform.h"

using namespace v8;

#define NewV8String(name) String::NewFromUtf8(Isolate::GetCurrent(), name,NewStringType::kNormal).ToLocalChecked()

namespace FQ {

    /*
       Objective-C type  |   JavaScript type        |       Java type
     --------------------+--------------------------+---------------------
             nil         |     undefined            |          null
            NSNull       |        null              |          null
           NSString      |       string             |         String
           NSNumber      |   number, boolean        |       double,boolean
         NSDictionary    |   Object object          |          HashMap
           NSArray       |    Array object          |          ArrayList
            NSDate       |     Date object          |           Date
           NSBlock       |   Function object        |          JSFunction
              id         |   Wrapper object         |           Object
            Class        | Constructor object       |           Class
            NSData       |      ArrayBuffer         |           byte[]  not implemented
     */
    class V8ScriptEngine {
    public:
        V8ScriptEngine(const char *defaultPath);

        Isolate::CreateParams *create_params;

        virtual ~V8ScriptEngine();

    private:
        Platform *platform;

    };

}
#endif /* V8ScriptEngine_hpp */
