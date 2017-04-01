package com.liam.library.v8;

public class V8Exception extends RuntimeException {

	private static final long serialVersionUID = 1L;

	public V8Exception(String message) {
		super(message);
	}

	public V8Exception(Throwable cause) {
		super(cause);
	}

	public V8Exception(String message, Throwable cause) {
		super(message, cause);
	}

	public V8Exception(String message, Throwable cause, boolean enableSuppression, boolean writableStackTrace) {
		super(message, cause, enableSuppression, writableStackTrace);
	}

}
