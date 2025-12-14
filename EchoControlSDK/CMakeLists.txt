cmake_minimum_required(VERSION 3.15)
project(EchoControlSDK LANGUAGES CXX)

# -------------------------------
# C++ 标准
# -------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# 自动收集源文件和头文件
# -------------------------------
file(GLOB_RECURSE ECHO_SDK_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE ECHO_SDK_HEADERS
    CONFIGURE_DEPENDS
    src/*.h
    src/*.hpp
)

# -------------------------------
# 输出目录
# -------------------------------
set(LIB_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)

# -------------------------------
# 静态库
# -------------------------------
add_library(EchoControlSDK STATIC
    ${ECHO_SDK_SOURCES}
    ${ECHO_SDK_HEADERS}
)

# 静态库输出到 lib/static
set_target_properties(EchoControlSDK PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}/static
    OUTPUT_NAME "EchoControlSDK"  # 静态库名
)

target_include_directories(EchoControlSDK
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# -------------------------------
# 动态库
# -------------------------------
add_library(EchoControlSDK_shared SHARED
    ${ECHO_SDK_SOURCES}
    ${ECHO_SDK_HEADERS}
)

# 动态库输出到 lib/shared（DLL 和 import lib 都在此目录）
set_target_properties(EchoControlSDK_shared PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}/shared    # DLL
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}/shared    # import lib
    OUTPUT_NAME "EchoControlSDK"                          # DLL 名
)

# DLL 导出宏
target_compile_definitions(EchoControlSDK_shared PRIVATE ECHOCONTROLSDK_EXPORTS)

target_include_directories(EchoControlSDK_shared
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# -------------------------------
# Windows 网络库
# -------------------------------
if (WIN32)
    target_link_libraries(EchoControlSDK ws2_32)
    target_link_libraries(EchoControlSDK_shared ws2_32)
endif()
