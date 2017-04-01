/*
 * Config.h
 *
 *  Created on: 2015年6月1日
 *      Author: liaomin
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <string>

#include "v8.h"
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "native", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native", __VA_ARGS__))

#define DELETE_POINTER(t) if(t){delete t;t=nullptr;}

#define JAVA_V8_JS_ENGINE_CLASS "com/fq/liam/javascript/engine/V8JSEngine"

#endif /* CONFIG_H_ */
