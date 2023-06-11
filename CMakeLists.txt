# cmake_minimum_required(VERSION 3.0)
# PROJECT(ConnectionPool_SYSTEM)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_BUILD_TYPE "DEBUG"
# )

# # set(lib_DIR /usr/lib/x86_64-linux-gnu)
# # find_package(${lib_DIR})
# # target_link_libraries(ConnectionPool_SYSTEM libjsoncpp.a)


# # set output binary path  
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# # include_directories(${CMAKE_SOURCE_DIR}/include)

# # set(SRC 
# #     src/ConnectionPool.cpp 
# #     src/MysqlConn.cpp
# # )
# # add_executable(solider_cmake main.cpp src/ConnectionPool.cpp  src/MysqlConn.cpp)
# # add_executable(main main.cpp ${SRC})
# add_executable(
#     SQLConnetionPool main.cpp 
#                include/MysqlConn.h src/MysqlConn.cpp
#                include/ConnectionPool.h src/ConnectionPool.cpp  
# )

cmake_minimum_required(VERSION 3.0)
project(connectionpoll)
# CMAKE_CXX_FLAGS g++编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
set(CMAKE_CXX_STANDARD 20)



set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 向工程添加多个特定的头文件搜索路径 --->相当于指定g++编译器的-I参数
include_directories(${PROJECT_SOURCE_DIR}/include)

# 向当前工程添加存放源文件的子目录，并可以指定中间二进制和目标二进制存放的位置
# 添加 src 子目录，src 中必须有一个 CMakeLists.txt
add_subdirectory(src)