package com.liam.library.v8;

import java.util.HashMap;

/**
 * add a strong reference for an object use HashMap
 * System.identityHashCode(object) as key to cache java object
 * @author liaomin 
 */
public class JavaReference {
	
	private static HashMap<Integer, Object> mReferences = new HashMap<Integer, Object>();
	private static int index = 0;


	private static int generateIdentity(){
		while (true){
			int identity = index++;
			if(identity == Integer.MAX_VALUE){
				index = 1;
				identity = index;
			}
			if(!mReferences.containsKey(identity)){
				return identity;
			}
		}
	}
	
	public static int addRefrence(Object object){
		if(object != null){
//			int key = System.identityHashCode(object);
			int key = generateIdentity();
			mReferences.put(key, object);
			return key;
		}
		return 0;
	}

	
	public static Object removeReference(int key){
		return mReferences.remove(key);
	}
	
	public static Object getObject(int key){
		return mReferences.get(key);
	}

	
	public static long invokeJsFunction(int contextKey,int key,long[] args,long jsCallee,long jsThis){
		Object context = mReferences.get(contextKey);
		Object result = null;
		if(context != null && context instanceof JSContext){
			JSContext.setArguments(args,jsCallee,jsThis);
			Object value = mReferences.get(key);
			if(value == null) throw new RuntimeException("Object is released for key:"+key);
			if(value != null && value instanceof JSFunction){
				result =  ((JSFunction)value).onInvoke();
			}
			JSContext.removeCurrentArguments();
		}
		JSValue res = JSValue.valueWithObject(JSContext.currentContext(),result);
		long ni = res.nativeIdentity;
		res.justRemoveJavaRefrence();
		return ni;
	}

	public static void releaseJavaRef(int identity){
		mReferences.remove(identity);
//		Log.e("","--->"+mReferences.size());
	}

//	public static boolean isSameType(Class class1,Object object){
//		if(class1.isInstance(object)) return true;
//		Class class2 = object.getClass();
//		String name1 = class1.getName();
//		return false;
//	}
//	
//	public static boolean checkType(Class[] params,Object...args){
//		if(params.length != args.length) return false;
//		for (int i = 0; i < args.length; i++) {
//		}
//		return true;
//	}
//
//	public static Object[] toObjects(long[] args){
//		Object[] objects = new Object[args.length];
//		for (int i = 0; i < args.length; i++) {
//			objects[i] = JSValue.poll(JSContext.currentContext(), args[i]).toObject();
//		}
//		return objects;
//	}
//	
}
