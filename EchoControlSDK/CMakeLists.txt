cmake_minimum_required(VERSION 3.15)
project(EchoControlSDK LANGUAGES CXX)

# ==============================================================================
# 基础配置 (平台、架构、输出路径)
# ==============================================================================
if (WIN32)
    set(ECCS_PLATFORM windows)
elseif (UNIX)
    set(ECCS_PLATFORM linux)
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
# 第三方库配置 (华视芯 HSX SDK)
# ==============================================================================
set(HSX_ROOT_DIR ${CMAKE_SOURCE_DIR}/3rd/hsx_vision)

if (WIN32)
    # --- Windows 配置 ---
    set(HSX_INCLUDE_DIR ${HSX_ROOT_DIR}/win/include)
    set(HSX_LIB_DIR     ${HSX_ROOT_DIR}/win/lib)
    set(HSX_BIN_DIR     ${HSX_ROOT_DIR}/win/bin)

    # 指定需要链接的 .lib 文件 (导入库)
    set(HSX_LIBS 
        ${HSX_LIB_DIR}/AjNetSdkDll.lib
        ${HSX_LIB_DIR}/AjPlayer.lib
        ${HSX_LIB_DIR}/AjRtspClientLib.lib
        ${HSX_LIB_DIR}/RemoteConfig.lib
    )
    add_definitions(-DPLATFORM_WIN32)

elseif (UNIX)
    # --- Linux 配置 ---
    set(HSX_INCLUDE_DIR ${HSX_ROOT_DIR}/linux/include)
    set(HSX_LIB_DIR     ${HSX_ROOT_DIR}/linux/lib/x86_64)

    # 链接静态库 (.a) 以实现单文件输出
    # 注意：静态库链接顺序通常很重要，底层库放在后面
    set(HSX_LIBS 
        ${HSX_LIB_DIR}/libNetSDK.a
        ${HSX_LIB_DIR}/libixml.a
        ${HSX_LIB_DIR}/libtinyxml.a
    )
    add_definitions(-DPLATFORM_LINUX)
endif()

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
target_include_directories(EchoControlSDK 
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
                            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                            PRIVATE ${HSX_INCLUDE_DIR}
                            )

# --- 动态库 (DLL) ---
add_library(EchoControlSDK_shared SHARED ${ECHO_SDK_SOURCES} ${ECHO_SDK_HEADERS})

set_target_properties(EchoControlSDK_shared PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/bin"   # DLL 输出到 bin
    ARCHIVE_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/lib"   # Import Lib 输出到 lib
    LIBRARY_OUTPUT_DIRECTORY "${ECCS_OUTPUT_ROOT}/bin"   # SO 输出到 bin
    OUTPUT_NAME "EchoControlSDK"
)
target_compile_definitions(EchoControlSDK_shared PRIVATE ECHOCONTROLSDK_EXPORTS)
target_include_directories(EchoControlSDK_shared 
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
                            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                            PRIVATE ${HSX_INCLUDE_DIR}
                            )

# --- 系统库链接 ---
if (WIN32)
    target_link_libraries(EchoControlSDK PRIVATE ${HSX_LIBS} ws2_32)
    target_link_libraries(EchoControlSDK_shared PRIVATE ${HSX_LIBS} ws2_32)
elseif (UNIX)
    target_link_libraries(EchoControlSDK PRIVATE ${HSX_LIBS} pthread d1)
    target_link_libraries(EchoControlSDK_shared PRIVATE ${HSX_LIBS} pthread d1)
endif()

# ==============================================================================
# Windows 特有逻辑：自动拷贝厂商 DLL 到 SDK 输出目录
# ==============================================================================
if (WIN32)
    # 收集 3rd/hsx_vision/win/bin 下的所有 dll
    file(GLOB HSX_DLL_FILES "${HSX_BIN_DIR}/*.dll")
    
    foreach(DLL_PATH ${HSX_DLL_FILES})
        add_custom_command(TARGET EchoControlSDK_shared POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL_PATH}
            $<TARGET_FILE_DIR:EchoControlSDK_shared>
            COMMENT "Copying HSX runtime DLL: ${DLL_PATH}"
        )
    endforeach()
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
# 测试程序: EchoControlTest
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