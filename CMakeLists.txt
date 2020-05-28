#CMake最低版本号要求
cmake_minimum_required(VERSION 2.8)
#项目信息,工程顶层目录名称
project(luabind)

#使用C++ 11标准
set(CMAKE_CXX_STANDARD 11)

# C99 标准
#set(CMAKE_C_STANDARD 99)

# 启用调试/发版模式
#SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_BUILD_TYPE Debug)
# 显示编译过程
set(CMAKE_VERBOSE_MAKEFILE ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(LUA_INCLUDE_PATH /data1/apache2/htdocs/LuaJIT/src/)
set(LUA_LIB_PATH /data1/apache2/htdocs/LuaJIT/src/)

include_directories(${LUA_INCLUDE_PATH})
link_directories(${LUA_LIB_PATH})

# 自动发现目录下的源码
#AUX_SOURCE_DIRECTORY(. RPC_SOURCE)

set(RPC_SOURCE
        bind.cpp
)

#开启后,默认生成的为动态库
#SET(BUILD_SHARED_LIBS ON)

# 指定生成的类库
add_library(luabind SHARED ${RPC_SOURCE})
TARGET_LINK_LIBRARIES(luabind luajit)

#指定生成目标
add_executable(test test.cpp)
TARGET_LINK_LIBRARIES(test luabind)


# make test
ADD_TEST(mytest ${PROJECT_BINARY_DIR}/bin/test)
ENABLE_TESTING()
