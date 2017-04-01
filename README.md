### install 
```javascript
 allprojects {
		repositories {
			...
			maven { url 'https://jitpack.io' }
		}
	}
  
  dependencies {
	        compile 'com.github.liaomin:v8jsc:v0.1.1'
	}
 ```
 
### example
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
                JSValue o = context.evaluateScript("'2+3'");
                assertEquals(o.toObject(),5);
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
