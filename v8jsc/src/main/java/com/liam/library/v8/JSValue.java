package com.liam.library.v8;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Locale;
import java.util.concurrent.ConcurrentLinkedQueue;

public class JSValue {

	/**
	 * v8 Value identity, use this identity to find persistent v8 Value
	 */
	protected long nativeIdentity = 0;
	
	/**
	 * java reference count ,if count == 0 v8 Value will be weak
	 */
	private int referenceCount = 0;
	
	/**
	 * current context
	 */
	protected JSContext context;
	
	/**
	 * 
	 */
	private static ConcurrentLinkedQueue<JSValue> cached;
	
	
	private static ConcurrentLinkedQueue<JSValue> used;
	
	
	private static final int MAX_CACHED_SIZE = 100;
	
	static{
		cached = new ConcurrentLinkedQueue<JSValue>();
		used = new ConcurrentLinkedQueue<JSValue>();
	}
	
	public static void clean(){
		Iterator<JSValue> iterator = used.iterator();
		while (iterator.hasNext()) {
			JSValue value = (JSValue) iterator.next();
			value.removeRefrence();
		}
	}
	
	public static JSValue poll(JSContext context , long nPtr){
		if(nPtr == 0 || context == null) return null;
		JSValue cJsValue = cached.poll();
		if(cJsValue == null){
			cJsValue = new JSValue();
		}
		used.offer(cJsValue);
		cJsValue.nativeIdentity = nPtr;
		cJsValue.context = context;
		cJsValue.addRefrence();
		return cJsValue;
	}
	
	protected JSValue() {
		
	}

	
	public static JSValue valueWithObject(JSContext context,Object value){
		if(context != null && context.nativeIdentity != 0){
			context.check();
			if(value == null){
				return JSValue.poll(context, nativeNewNull(context.nativeIdentity));
			}
			if(value instanceof JSValue){
				return (JSValue) value;
			}
			if(value instanceof Integer){
				return JSValue.poll(context,nativeNewInt(context.nativeIdentity, (int)value));
			}
			if(value instanceof Double || value instanceof Float){
				return JSValue.poll(context,nativeNewDouble(context.nativeIdentity, (double)value));
			}
			if(value instanceof String){
				return JSValue.poll(context,nativeNewString(context.nativeIdentity, (String)value));
			}
			if(value instanceof Boolean){
				return JSValue.poll(context,nativeNewBool(context.nativeIdentity, (boolean)value));
			}
			if(value instanceof JSFunction){
				return JSValue.poll(context,nativeNewFunction(context.nativeIdentity, context.javaReference,JavaReference.addRefrence(value)));
			}
			return JSValue.poll(context, nativeNewJaveObject(context.nativeIdentity, context.javaReference, JavaReference.addRefrence(value)));
		}
		return null;
	}
	
	public static JSValue valueWithBool(JSContext context,boolean value){
		context.check();
		return JSValue.poll(context, nativeNewBool(context.nativeIdentity, value));
	}
	
	public static JSValue valueWithString(JSContext context,String value){
		context.check();
		return JSValue.poll(context, nativeNewString(context.nativeIdentity, value));
	}
	
	public static JSValue valueWithDouble(JSContext context,double value){
		context.check();
		return JSValue.poll(context, nativeNewDouble(context.nativeIdentity, value));
	}
	
	public static JSValue valueWithInt(JSContext context,int value){
		context.check();
		return JSValue.poll(context, nativeNewInt(context.nativeIdentity, value));
	}
	
	public static JSValue valueWithNewObjectInContext(JSContext context){
		context.check();
		return JSValue.poll(context, nativeNewJSObject(context.nativeIdentity));
	}
	
	public static JSValue valueWithNewArrayInContext(JSContext context){
		context.check();
		return JSValue.poll(context, nativeNewJSArray(context.nativeIdentity));
	}
	
	public static JSValue valueWithNewRegularExpressionFromPattern(JSContext context,String pattern,String flags){
		context.check();
		int flag = 0 ;
		if(flags != null){
			flags = flags.toLowerCase(Locale.getDefault());
			if(flags.contains("g")){
				flag |= 1;
			}
			if(flags.contains("i")){
				flag |= 2;
			}
			if(flags.contains("m")){
				flag |= 4;
			}
			if(flags.contains("s")){
				flag |= 8;
			}
			if(flags.contains("u")){
				flag |= 16;
			}
		}
		return JSValue.poll(context, nativeNewRegex(context.nativeIdentity,pattern,flag));
	}
	
	public static JSValue valueWithNewErrorFromMessage(JSContext context,String message){
		context.check();
		return JSValue.poll(context, nativeNewError(context.nativeIdentity,message));
	}
	
	public static JSValue valueWithNullInContext(JSContext context){
		context.check();
		return JSValue.poll(context, nativeNewNull(context.nativeIdentity));
	}
	
	public static JSValue valueWithUndefinedInContext(JSContext context){
		context.check();
		return JSValue.poll(context, nativeNewUndefined(context.nativeIdentity));
	}
	
	public Object toJavaObject(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope)
			return nativeToObject(context.nativeIdentity,nativeIdentity);
		return null;
	}
	
	public Object toObject(){
		return JSConver.converToObject(this);
	}
	
	public boolean toBool(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope)
			return nativeToBool(context.nativeIdentity,nativeIdentity);
		return false;
	}
	
	public double toDouble(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope)
			return nativeToDouble(context.nativeIdentity,nativeIdentity);
		return 0;
	}
	
	public int toInt(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope )
			return nativeToInt(context.nativeIdentity,nativeIdentity);
		return 0;
	}
	
	@Override
	public String toString(){
		boolean isJavaObject = isJavaObject();
		if(!isJavaObject && context != null && nativeIdentity != 0){
			if( context.isEnterScope)
				return nativeToString(context.nativeIdentity,nativeIdentity);
			return "context not enter scope";
		}
		if(isJavaObject){
			return toJavaObject().toString();
		}
		return super.toString();
	}

	public Date toDate(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope){
			long timestamp = nativeToDate(context.nativeIdentity,nativeIdentity);
			if(timestamp > 0){
				return new Date(timestamp);
			}
		}
		return null;
	}
	
	public ArrayList<Object> toArray(){
		Object object = JSConver.converToObject(this);
		if(object instanceof ArrayList){
			return (ArrayList<Object>) object;
		}
		return null;
	}
	
	public HashMap<String, Object> toDictionary(){
		Object object = JSConver.converToObject(this);
		if(object instanceof HashMap){
			return (HashMap<String, Object>) object;
		}
		return null;
	}
	
	
	public boolean hasProperty(String property){
		if(context != null && nativeIdentity != 0 && property != null && context.isEnterScope) return nativeHasProperty(context.nativeIdentity,nativeIdentity,property);
		return false;
	}

	public JSValue valueAtIndex(int index){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return poll(context, nativeValueAtIndex(context.nativeIdentity,nativeIdentity,index));
		return null;
	}
	
	public void setValue(int index,Object value){
		set(String.valueOf(index), value);
	}
	
	public boolean isUndefined(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsUndefined(context.nativeIdentity, nativeIdentity);
		return true;
	}
	
	public boolean isNull(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsNull(context.nativeIdentity, nativeIdentity);
		return true;
	}
	
	public boolean isBoolean(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsBoolean(context.nativeIdentity, nativeIdentity);
		return false;
	}
	
	public boolean isNumber(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsNumber(context.nativeIdentity, nativeIdentity);
		return false;
	}
	
	public boolean isString(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsString(context.nativeIdentity, nativeIdentity);
		return false;
	}
	
	public boolean isObject(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsObject(context.nativeIdentity, nativeIdentity);
		return false;
	}
	
	public boolean isArray(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsArray(context.nativeIdentity, nativeIdentity);
		return false;
	}
	
	public boolean isDate(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsDate(context.nativeIdentity, nativeIdentity);
		return false;
	}
	
	public boolean isJavaObject(){
		if(context != null && nativeIdentity != 0 && context.isEnterScope) return nativeIsJavaObject(context.nativeIdentity, nativeIdentity);
		return false;
	}
	
	public JSValue callWithArguments(Object... arguments){
		if(context != null){
			if(arguments != null){
				long[] args = new long[arguments.length];
				for (int i = 0; i < args.length; i++) {
					args[i] = JSValue.valueWithObject(context, arguments[i]).nativeIdentity;
				}
				return JSValue.poll(context, nativeCall(context.nativeIdentity, nativeIdentity, false, args));
			}
		}
		return null;
	}
	
	public JSValue constructWithArguments(Object... arguments){
		context.check();
		if(arguments != null){
			long[] args = new long[arguments.length];
			for (int i = 0; i < args.length; i++) {
				args[i] = JSValue.valueWithObject(context, arguments[i]).nativeIdentity;
			}
			return JSValue.poll(context, nativeCall(context.nativeIdentity, nativeIdentity, true, args));
		}
		return null;
	}
	
	
	/**
	 * Get a particular property on the global object.
	 * @param key
	 * @return The JSValue for the global object's property.
	 */
	public JSValue get(String key){
		if(context != null && nativeIdentity != 0 && key != null) return JSValue.poll(context, nativeGet(context.nativeIdentity,nativeIdentity,key));
		return null;
	}
	
	/**
	 * Set a particular property on the global object.
	 * @param key
	 * @param value
	 */
	public void set(String key,int value){
		if(context != null && nativeIdentity != 0 && key != null) 
			nativeSetInt(context.nativeIdentity, nativeIdentity, key, value);
	}
	
	/**
	 * Set a particular property on the global object.
	 * @param key
	 * @param value
	 */
	public void set(String key,double value){
		if(context != null && nativeIdentity != 0 && key != null) 
			nativeSetDouble(context.nativeIdentity, nativeIdentity, key, value);
	}

	/**
	 * Set a particular property on the global object.
	 * @param key
	 * @param value
	 */
	public void set(String key,String value){
		if(context != null && nativeIdentity != 0 && key != null) 
			nativeSetString(context.nativeIdentity, nativeIdentity, key, value);
	}
	
	/**
	 * Set a particular property on the global object.
	 * @param key
	 * @param value
	 */
	public void set(String key,boolean value){
		if(context != null && nativeIdentity != 0 && key != null) 
			nativeSetBoolean(context.nativeIdentity, nativeIdentity, key, value);
	}
	
	/**
	 * Set a particular property on the global object.
	 * @param key
	 * @param value
	 */
//	public void set(String key,JSFunction value){
//		if(context != null && nativeIdentity != 0 && key != null) {
//			nativeSetFunction(context.nativeIdentity, nativeIdentity, key, value,JavaReference.addRefrence(value),context.javaReference);
//		}
//	}
	
	/**
	 * Set a particular property on the global object.
	 * @param key
	 * @param value
	 */
	public void set(String key,Date value){
		if(context != null && nativeIdentity != 0 && key != null && value!= null) 
			nativeSetDate(context.nativeIdentity, nativeIdentity, key, value.getTime());
	}
	
	/**
	 * Set a particular property on the global object.
	 * @param key
	 * @param value
	 */
	public void set(String key,Object value){
//		if(context != null && nativePtr != 0 && key != null) 
//			nativeSetObject(context.nativePtr, nativePtr, key, value,JavaReference.addRefrence(value),context.javaReference);
		set(key, JSValue.valueWithObject(context, value));
	}
	
	public void set(String key,JSValue value){
		if(context != null && nativeIdentity != 0 && key != null && value != null && value.nativeIdentity != 0) {
			nativeSetJSValue(context.nativeIdentity, nativeIdentity, key, value.nativeIdentity);
		}
	}
	
	
	public JSValue setNativeAdress(long nativeAdress) {
		this.nativeIdentity = nativeAdress;
		return this;
	}
	
	
	public JSContext getContext() {
		return context;
	}

	public JSValue setContext(JSContext context) {
		this.context = context;
		return this;
	}

	public int getRefrenceCount() {
		return referenceCount;
	}

	public int addRefrence() {
		return ++referenceCount;
	}
	
	public void removeRefrence() {
		referenceCount--;
		used.remove(this);
		if(referenceCount <= 0){
			if(cached.size() < MAX_CACHED_SIZE)cached.add(this);
			if(context != null && nativeIdentity != 0 ) {// && !isJavaObject()){
				nativeRelease(context.nativeIdentity, nativeIdentity);
			}
			nativeIdentity = 0;
			context = null;
			referenceCount = 0;
		}
	}

	protected void justRemoveJavaRefrence() {
		referenceCount--;
		used.remove(this);
		if(referenceCount <= 0){
			if(cached.size() < MAX_CACHED_SIZE)cached.add(this);
			nativeIdentity = 0;
			context = null;
			referenceCount = 0;
		}
	}
	
	private static native void nativeRelease(long contextPtr,long ptr);
	private static native boolean nativeIsUndefined(long contextPtr,long ptr);
	private static native boolean nativeIsNull(long contextPtr,long ptr);
	private static native boolean nativeIsBoolean(long contextPtr,long ptr);
	private static native boolean nativeIsNumber(long contextPtr,long ptr);
	private static native boolean nativeIsString(long contextPtr,long ptr);
	private static native boolean nativeIsObject(long contextPtr,long ptr);
	private static native boolean nativeIsArray(long contextPtr,long ptr);
	private static native boolean nativeIsDate(long contextPtr,long ptr);
	private static native boolean nativeIsJavaObject(long contextPtr,long ptr);
	
	//get
	private static native long nativeGet(long contextPtr,long ptr,String key);
	
	//convert
	private static native String nativeToString(long nativePt2, long nativePtr2);
	private static native Object nativeToObject(long nativePt2, long nativePtr2);
	private static native boolean nativeToBool(long nativePt2, long nativePtr2);
	private static native double nativeToDouble(long nativePt2, long nativePtr2);
	private static native int nativeToInt(long nativePt2, long nativePtr2);
	private static native long nativeToDate(long nativePt2, long nativePtr2);
	
	//set
	private static native void nativeSetInt(long nativePtr, long nativePtr2,String key,int value);
	private static native void nativeSetDouble(long nativePtr, long nativePtr2,String key,double value);
	private static native void nativeSetString(long nativePtr, long nativePtr2,String key,String value);
	private static native void nativeSetBoolean(long nativePtr, long nativePtr2,String key,boolean value);
//	private static native void nativeSetFunction(long nativePtr, long nativePtr2,String key,JSFunction value,int k,int ck);
	private static native void nativeSetDate(long nativePtr, long nativePtr2,String key,long timestemp);
//	private static native void nativeSetObject(long nativePtr, long nativePtr2,String key,Object value,int k,int ck);
	private static native void nativeSetJSValue(long nativePtr, long nativePtr2,String key,long value);

	//new
	private static native long nativeNewJaveObject(long nativeContextPtr, int contextRefrence,int javaObjectReference);
	private static native long nativeNewBool(long nativeContextPtr, boolean value);
	private static native long nativeNewString(long nativeContextPtr, String value);
	private static native long nativeNewDouble(long nativeContextPtr, double value);
	private static native long nativeNewInt(long nativeContextPtr, int value);
	private static native long nativeNewJSObject(long nativeContextPtr);
	private static native long nativeNewJSArray(long nativeContextPtr);
	private static native long nativeNewRegex(long nativeContextPtr,String pattern,int flags);
	private static native long nativeNewError(long nativeContextPtr,String message);
	private static native long nativeNewNull(long nativeContextPtr);
	private static native long nativeNewUndefined(long nativeContextPtr);
	private static native long nativeNewFunction(long nativeContextPtr,long contextId,long functionId);
	
	private static native boolean nativeHasProperty(long nativeContextPtr,long nativePtr,String property);
	private static native long nativeValueAtIndex(long nativeContextPtr,long nativePtr,int index);
	private static native long nativeCall(long nativeContextPtr,long nativePtr,boolean isConstruct,long[] args);
	
}
