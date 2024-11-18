# chatgpt compile

# 设置最低版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和语言
project(Program LANGUAGES C)

# 设置编译器
set(CMAKE_C_COMPILER "/usr/bin/gcc")

# 添加编译选项
set(CMAKE_C_FLAGS "-Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -std=gnu99 -Wno-unused-function")

# 定义可执行文件名称
set(EXE_NAME "program")

# 定义源文件目录
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# 定义公共源文件
set(COMMON_SOURCES
    "${SRC_DIR}/common/print.c"
    "${SRC_DIR}/common/convert.c"
)

# 定义硬件相关源文件
set(CPU_SOURCES
    "${SRC_DIR}/headware/cpu/mmu.c"
    "${SRC_DIR}/headware/cpu/isa.c"
)

set(MEMORY_SOURCES
    "${SRC_DIR}/headware/memory/dram.c"
)

# 定义主文件
set(MAIN_SOURCE "${SRC_DIR}/tests/test_headware.c")

# 将所有源文件收集到一个列表中
set(SOURCES
    ${COMMON_SOURCES}
    ${CPU_SOURCES}
    ${MEMORY_SOURCES}
    ${MAIN_SOURCE}
)

# 添加可执行文件
add_executable(${EXE_NAME} ${SOURCES})

# 设置包含目录
target_include_directories(${EXE_NAME} PRIVATE ${SRC_DIR})

# 添加清理目标
add_custom_target(clean_target
    COMMAND ${CMAKE_COMMAND} -E remove ${EXE_NAME}
    COMMENT "Cleaning up generated files"
)
