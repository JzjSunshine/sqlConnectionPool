# 发现一个目录下所有源代码文件并将列表存储在一个变量中，这个指令临时被用来自动构建源文件列表
aux_source_directory(. SRC) #  定义SRC变量，其值为当前目录下所有的源代码文件

# 加入 main.cpp
file(GLOB_RECURSE main_src ${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp)

# 编译SRC变量所代表的源代码文件，生成main可执行文件
add_executable(ConnectionPool ${main_src} ${SRC})

set(lib_DIR /usr/lib/x84_64-linux-gnu)
link_directories(${lib_DIR})

target_link_libraries(ConnectionPool mysqlclient pthread libjsoncpp.a)
