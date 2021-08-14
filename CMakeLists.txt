cmake_minimum_required(VERSION 3.16)
project(Module_28_threads_task_1)

set(CMAKE_CXX_STANDARD 11)

option(ENABLE_TESTING "Enable test emulation" OFF)

add_executable(Module_28_threads_task_1)

target_sources(Module_28_threads_task_1
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        )

target_include_directories(Module_28_threads_task_1
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(Module_28_threads_task_1
        PRIVATE
        ENABLE_TESTING=$<BOOL:${ENABLE_TESTING}>
        )

