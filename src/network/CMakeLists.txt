# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

PROJECT(NETWORK_MOD) 

SET(SRC_LIST "posix_network.cpp asio_network.cpp io_uring_network.cpp coroutine_network.cpp")

# <projectname>_BINARY_DIR 和PROJECT_BINARY_DIR 变量，他们指的编译发生的当前目录，
# 如果是内部编译，就相当于 PROJECT_SOURCE_DIR 也就是工程代码所在目录，如果是外部编译，指的是外部编译所在目录，也就是本例中的 build 目录。
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})

# INSTALL(TARGETS myrun mylib mystaticlib 
# RUNTIME DESTINATION bin 
# LIBRARY DESTINATION lib
# ARCHIVE DESTINATION libstatic
# )
INSTALL(FILES COPYRIGHT README 
DESTINATION share/doc/cmake/t2
)
INSTALL(PROGRAMS runhello.sh 
DESTINATION bin
)
INSTALL(DIRECTORY doc/ 
DESTINATION share/doc/cmake/t2
)
#cmake -DCMAKE_INSTALL_PREFIX=/tmp/t2/usr ..  CMAKE_INSTALL_PREFIX 的默认定义是/usr/local



ADD_EXECUTABLE(network ${SRC_LIST})

# 将执行文件安装到bin目录
INSTALL(TARGETS network 
RUNTIME DESTINATION bin
)




