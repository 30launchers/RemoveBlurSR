cmake_minimum_required(VERSION 3.20)
project(RemoveBlurSR VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
)

add_library(RemoveBlurSR SHARED ${SOURCES})

target_include_directories(RemoveBlurSR PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../hideSRuid/src/MinHook/include
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_link_libraries(RemoveBlurSR PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MinHook/lib/libMinHook.x64.lib
    )
else()
    target_link_libraries(RemoveBlurSR PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MinHook/lib/libMinHook.x86.lib
    )
endif()

target_compile_definitions(RemoveBlurSR PRIVATE
    _CRT_SECURE_NO_WARNINGS
)

target_compile_options(RemoveBlurSR PRIVATE
    /utf-8
)

set_target_properties(RemoveBlurSR PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_options(RemoveBlurSR PRIVATE
    /NODEFAULTLIB:libcmt
    /LTCG
)
