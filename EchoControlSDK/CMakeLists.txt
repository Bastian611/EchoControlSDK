cmake_minimum_required(VERSION 3.15)
project(EchoControlSDK LANGUAGES CXX)

# -------------------------------
# 1. 平台与架构
# -------------------------------
if (WIN32)
    set(ECCS_PLATFORM windows)
elseif (UNIX)
    set(ECCS_PLATFORM linux)
else()
    set(ECCS_PLATFORM unknown)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ECCS_ARCH x64)
else()
    set(ECCS_ARCH x86)
endif()

# -------------------------------
# 2. 统一输出根目录
# -------------------------------
# 所有生成物都放在项目根目录下的 out/windows/x64/ 结构中
set(ECCS_OUTPUT_ROOT
    ${CMAKE_SOURCE_DIR}/out/${ECCS_PLATFORM}/${ECCS_ARCH}
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# 3. 收集源文件
# -------------------------------
file(GLOB_RECURSE ECHO_SDK_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE ECHO_SDK_HEADERS
    CONFIGURE_DEPENDS
    src/*.h
    src/*.hpp
    include/*.h # 包含对外接口头文件
)

# -------------------------------
# 4. 静态库 Target
# -------------------------------
add_library(EchoControlSDK STATIC
    ${ECHO_SDK_SOURCES}
    ${ECHO_SDK_HEADERS}
)

# [核心修复] 使用 ECCS_OUTPUT_ROOT 统一路径
set_target_properties(EchoControlSDK PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/lib_static"
    OUTPUT_NAME "EchoControlSDK"
)

# 防止 C2491 报错 (静态库也定义导出宏，骗过编译器)
target_compile_definitions(EchoControlSDK PRIVATE ECHOCONTROLSDK_EXPORTS)

target_include_directories(EchoControlSDK
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------------
# 5. 动态库 Target
# -------------------------------
add_library(EchoControlSDK_shared SHARED
    ${ECHO_SDK_SOURCES}
    ${ECHO_SDK_HEADERS}
)

set_target_properties(EchoControlSDK_shared PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/bin"   # DLL
    ARCHIVE_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/lib"   # LIB (导入库)
    OUTPUT_NAME "EchoControlSDK"
)

target_compile_definitions(EchoControlSDK_shared PRIVATE ECHOCONTROLSDK_EXPORTS)

target_include_directories(EchoControlSDK_shared
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------------
# 6. 链接库
# -------------------------------
if (WIN32)
    target_link_libraries(EchoControlSDK ws2_32)
    target_link_libraries(EchoControlSDK_shared ws2_32)
elseif (UNIX)
    target_link_libraries(EchoControlSDK pthread)
    target_link_libraries(EchoControlSDK_shared pthread)
endif()