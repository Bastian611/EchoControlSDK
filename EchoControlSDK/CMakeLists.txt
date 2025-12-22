cmake_minimum_required(VERSION 3.15)
project(EchoControlSDK LANGUAGES CXX)

# ==============================================================================
# 基础配置 (平台、架构、输出路径)
# ==============================================================================
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

# 统一输出根目录: out/windows/x64/
set(ECCS_OUTPUT_ROOT ${CMAKE_SOURCE_DIR}/out/${ECCS_PLATFORM}/${ECCS_ARCH})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================================================================
# SDK 库构建 (DLL & Static Lib)
# ==============================================================================

# 自动收集 src 下的所有源文件
file(GLOB_RECURSE ECHO_SDK_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE ECHO_SDK_HEADERS CONFIGURE_DEPENDS src/*.h src/*.hpp include/*.h)

# --- 静态库 ---
add_library(EchoControlSDK STATIC ${ECHO_SDK_SOURCES} ${ECHO_SDK_HEADERS})

set_target_properties(EchoControlSDK PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/lib_static"
    OUTPUT_NAME "EchoControlSDK"
)
# 定义导出宏
target_compile_definitions(EchoControlSDK PRIVATE ECHOCONTROLSDK_EXPORTS)
target_include_directories(EchoControlSDK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- 动态库 (DLL) ---
add_library(EchoControlSDK_shared SHARED ${ECHO_SDK_SOURCES} ${ECHO_SDK_HEADERS})

set_target_properties(EchoControlSDK_shared PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/bin"   # DLL 输出到 bin
    ARCHIVE_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/lib"   # Import Lib 输出到 lib
    OUTPUT_NAME "EchoControlSDK"
)
target_compile_definitions(EchoControlSDK_shared PRIVATE ECHOCONTROLSDK_EXPORTS)
target_include_directories(EchoControlSDK_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- 系统库链接 ---
if (WIN32)
    target_link_libraries(EchoControlSDK ws2_32)
    target_link_libraries(EchoControlSDK_shared ws2_32)
elseif (UNIX)
    target_link_libraries(EchoControlSDK pthread)
    target_link_libraries(EchoControlSDK_shared pthread)
endif()

# ==============================================================================
# 工具构建: ConfigTool (配置生成器)
# ==============================================================================
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tool/ConfigTool.cpp")
    add_executable(ConfigTool tool/ConfigTool.cpp)
    
    # 输出到 bin 目录，方便直接运行生成配置
    set_target_properties(ConfigTool PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/bin"
    )
    
    # 这是一个独立工具，通常不需要链接 SDK，只需要标准库
    # 如果用到了 SDK 里的 DeviceID 定义，可能需要 include 路径
    target_include_directories(ConfigTool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()

# ==============================================================================
# 4. 测试程序: EchoControlTest
# ==============================================================================
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp")
    add_executable(EchoControlTest test/main.cpp)

    # 输出到 bin 目录，这样可以直接加载同目录下的 DLL
    set_target_properties(EchoControlTest PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/bin"
        OUTPUT_NAME "TestApp"
    )

    # 链接 SDK 动态库
    target_link_libraries(EchoControlTest PRIVATE EchoControlSDK_shared)
    
    # 包含头文件路径
    target_include_directories(EchoControlTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    
    # (Windows) 确保测试程序生成后，将 DLL 所在目录加到环境路径方便调试（VS环境自动处理，这里主要用于标记依赖）
    add_dependencies(EchoControlTest EchoControlSDK_shared)
endif()