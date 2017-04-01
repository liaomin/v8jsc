package com.liam.library.v8;

import java.util.ArrayList;
import java.util.HashMap;

public class JSConver {
	
	/**
	 * null or undefined
	 */
	public static final int JSTYPE_NULL = 0;
	
	public static final int JSTYPE_STRING = 1;
	
	public static final int JSTYPE_NUMBER = 2;
	
	public static final int JSTYPE_ARRAY = 3;
	
	public static final int JSTYPE_DATE = 4;
	
	public static final int JSTYPE_JAVAOBJECT = 5;
	
	public static final int JSTYPE_BOOLEAN = 6;
	
	public static final int JSTYPE_JSOBJECT = 7;

	public static final int JSTYPE_JSFUNCTION = 8;

	public static Object converToObject(JSValue value){
		int jsType = getJSType(value);
		switch (jsType) {
		case JSTYPE_STRING:
			return value.toString();
		case JSTYPE_NUMBER:
			return value.toDouble();
		case JSTYPE_DATE:
			return value.toDate();
		case JSTYPE_JAVAOBJECT:
			return value.toJavaObject();
		case JSTYPE_ARRAY:
			return toArrayList(value);
		case JSTYPE_JSOBJECT:
			return toHashMap(value);
		case JSTYPE_BOOLEAN:
			return value.toBool();
		case JSTYPE_JSFUNCTION:
			return value.toString();
		default:
			return null;
		}
	}
	
	public static int getJSType(JSValue value){
		if(value != null && value.context != null && value.nativeIdentity != 0){
			return nativeGetJSType(value.context.nativeIdentity,value.nativeIdentity);
		}
		return JSTYPE_NULL;
	}
	
	
	private static HashMap<String, Object> toHashMap(JSValue value){
		if(value != null && value.context != null && value.nativeIdentity != 0){
			JSValue names = JSValue.poll(value.context, nativeGetObjectKeys(value.context.nativeIdentity,value.nativeIdentity));
			long array[] = nativeGetArrayValues(names.context.nativeIdentity,names.nativeIdentity);
			if(array != null){
				HashMap<String, Object> map = new HashMap<String, Object>();
				for (int j = 0; j < array.length; j++) {
					String key = JSValue.poll(value.context,array[j]).toString();
					JSValue value2 = value.get(key);
					map.put(key, converToObject(value2));
				}
				return map;
			}
		}
		return null;
	}
	
	private static ArrayList<Object> toArrayList(JSValue value){
		if(value != null && value.context != null && value.nativeIdentity != 0){
			long array[] = nativeGetArrayValues(value.context.nativeIdentity,value.nativeIdentity);
			if(array != null){
				ArrayList<Object> arrayList = new ArrayList<Object>(array.length);
				for (int i = 0; i < array.length; i++) {
					JSValue vJsValue = JSValue.poll(value.context, array[i]);
					arrayList.add(converToObject(vJsValue));
				}
				return arrayList;
			}
		}
		return null;
	}
	
	private static native int nativeGetJSType(long nativeContextPtr, long nativeValuePtr);
	private static native long[] nativeGetArrayValues(long nativeContextPtr, long nativeValuePtr);
	private static native long nativeGetObjectKeys(long nativeContextPtr, long nativeValuePtr);
}
