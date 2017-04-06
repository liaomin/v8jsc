### Gradle Dependency
Add the following to your module's `build.gradle` file:
```groovy
  dependencies {
	       	 compile 'com.liam.library:v8jsc:v0.1.1'
		}
 ```
 
### Usage
```java
JSContext.currentContext().handleScope(new JSContext.HandleScopeBlock() {
            @Override
            public void onEnterHandleScope(JSContext context) {
                JSValue globale =  context.getGlobalObject();
                context.evaluateScript("str = 'test';num=2.3;obj={e:1};arr=[{r:1},1]");
                assertEquals(globale.get("str").toString(),"test");
                assertEquals(globale.get("num").toObject(),2.3);
                assertTrue(globale.get("obj").isObject());
                assertTrue(globale.get("obj").toObject() instanceof HashMap);
                assertTrue(globale.get("arr").isArray());
                assertTrue(globale.get("arr").isObject());
                ArrayList d = (ArrayList)globale.get("arr").toObject();
                assertTrue(globale.get("arr").toObject() instanceof ArrayList);
                JSValue o = context.evaluateScript("2+3");
                assertEquals(o.toObject(),5.0);
                globale.set("javaFunction", new JSFunction() {
                    @Override
                    public Object onInvoke() {
                        List<JSValue> args = JSContext.currentArguments();
                        assertTrue(args.size()  == 1);
                        assertEquals(args.get(0).toString(),"arg");
                        return "javaReturnValue";
                    }
                });
                o =  context.evaluateScript("javaFunction('arg')");
                assertEquals(o.toObject(),"javaReturnValue");
            }
        });        
```
### Notice
 Include armeabi and x86 native library by default.add ndk { abiFilters 'armeabi'} to choose one
 
 
License
-------
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
       http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
