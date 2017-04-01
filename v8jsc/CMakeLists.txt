# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/include)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             V8Library

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/V8Library.cpp
             src/main/cpp/JniHelper.cpp
             src/main/cpp/V8Context.cpp
             src/main/cpp/V8ScriptEngine.cpp)



# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

add_library(libv8_base STATIC IMPORTED )
set_target_properties(libv8_base PROPERTIES IMPORTED_LOCATION ../../../../src/main/cpp/${ANDROID_ABI}/libv8_base.a)

add_library(libv8_libplatform STATIC IMPORTED )
set_target_properties(libv8_libplatform PROPERTIES IMPORTED_LOCATION ../../../../src/main/cpp/${ANDROID_ABI}/libv8_libplatform.a)

add_library(libv8_nosnapshot STATIC IMPORTED )
set_target_properties(libv8_nosnapshot PROPERTIES IMPORTED_LOCATION ../../../../src/main/cpp/${ANDROID_ABI}/libv8_nosnapshot.a)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log
              android
              stdc++
              atomic
              m)

target_link_libraries( # Specifies the target library.
                       V8Library
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       libv8_base
                       libv8_libplatform
                       libv8_nosnapshot )
