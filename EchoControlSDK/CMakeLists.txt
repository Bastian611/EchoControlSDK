cmake_minimum_required(VERSION 3.15)
project(EchoControlSDK LANGUAGES CXX)

# -------------------------------
# 平台名
# -------------------------------
if (WIN32)
    set(ECCS_PLATFORM windows)
elseif (UNIX)
    set(ECCS_PLATFORM linux)
else()
    set(ECCS_PLATFORM unknown)
endif()

# -------------------------------
# 架构名
# -------------------------------
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ECCS_ARCH x64)
else()
    set(ECCS_ARCH x86)
endif()

# -------------------------------
# 输出根目录
# -------------------------------
set(ECCS_OUTPUT_ROOT
    ${CMAKE_SOURCE_DIR}/out/lib/${ECCS_PLATFORM}/${ECCS_ARCH}
)

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
elseif (UNIX)
    # Linux下必须链接 pthread，否则 std::thread 会崩溃
    target_link_libraries(EchoControlSDK pthread)
    target_link_libraries(EchoControlSDK_shared pthread)
endif()
