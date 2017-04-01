package fq.v8jsc_app;

import android.os.Bundle;
import android.os.Debug;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import com.liam.library.v8.JSContext;
import com.liam.library.v8.JSFunction;
import com.liam.library.v8.JSValue;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.postDelayed(new Runnable() {
            @Override
            public void run() {
                final  Runnable self = this;
                JSContext.currentContext().handleScope(new JSContext.HandleScopeBlock() {
                    @Override
                    public void onEnterHandleScope(JSContext context) {
                        tv.postDelayed(self,10);
                        tv.setText(""+ (Debug.getNativeHeapSize()/(float)(1024*1024))+"m "
                                + (Debug.getNativeHeapAllocatedSize()/(float)(1024*1024))+"m "
                                + (Debug.getNativeHeapFreeSize()/(float)(1024*1024))+"m ");
                        context.evaluateScript("for(var i = 0 ; i < 10000 ; i++){ var o = test();};");
                    }
                });
            }
        },2000);
        JSContext.currentContext().handleScope(new JSContext.HandleScopeBlock() {
            @Override
            public void onEnterHandleScope(JSContext context) {
                context.setExceptionHandler(new JSContext.ExceptionHandler() {
                    @Override
                    public void onException(JSContext context, JSValue exception) {
                        Log.e("",exception.toString());
                    }
                });
                context.evaluateScript("e={es:10}");
                context.getGlobalObject().set("de",new Object());
                context.getGlobalObject().set("test", new JSFunction() {
                    @Override
                    public Object onInvoke() {
                        return new Object();
                    }
                });
                tv.setText(context.getGlobalObject().get("e").toString());
            }
        });
    }
}
