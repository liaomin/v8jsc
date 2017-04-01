package fq.v8jsc;

import android.support.test.runner.AndroidJUnit4;

import com.liam.library.v8.JSContext;
import com.liam.library.v8.JSFunction;
import com.liam.library.v8.JSValue;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    @Test
    public void testSetFunction() throws Exception {
        JSContext.currentContext().handleScope(new JSContext.HandleScopeBlock() {
            @Override
            public void onEnterHandleScope(JSContext context) {
                JSValue globale =  context.getGlobalObject();
                globale.set("log", new JSFunction() {
                    @Override
                    public Object onInvoke() {
                        List<JSValue> args = JSContext.currentArguments();
                        for(int i = 0 ; i < args.size() ; i ++){
                            assertEquals(args.get(i).toString(),"2");
                        }
                        return "success";
                    }
                });
                JSValue o = context.evaluateScript("log(2);");
                assertEquals(o.toString(),"success");
            }
        });
    }

    @Test
    public void testConver() throws Exception {
        JSContext.currentContext().handleScope(new JSContext.HandleScopeBlock() {
            @Override
            public void onEnterHandleScope(JSContext context) {
                JSValue globale =  context.getGlobalObject();
                context.evaluateScript("str = 'test';num=2.3;obj={e:1};arr=[{r:1},1]");
                assertEquals(globale.get("str").toString(),"test");
                assertEquals(globale.get("num").toString(),"2.3");
                assertTrue(globale.get("obj").isObject());
                assertTrue(globale.get("obj").toObject() instanceof HashMap);
                assertTrue(globale.get("arr").isArray());
                assertTrue(globale.get("arr").isObject());
                ArrayList d = (ArrayList)globale.get("arr").toObject();
                assertTrue(globale.get("arr").toObject() instanceof ArrayList);
                JSValue o = context.evaluateScript("'success'");
                assertEquals(o.toString(),"success");
            }
        });
    }

}
