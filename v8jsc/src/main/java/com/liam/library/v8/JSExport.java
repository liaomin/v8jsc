package com.liam.library.v8;

import java.lang.annotation.ElementType;
import java.lang.annotation.Target;

/**
 * not implemented yet
 * @author liaomin
 *
 */
public interface JSExport {
	
	@Target(ElementType.FIELD)
	public @interface JSExportAs{
		
	}
	
}
