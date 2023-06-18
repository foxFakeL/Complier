# 声明模块名称和源文件列表
set(MODULE_NAME VM)
set(MODULE_SOURCES Machine.h Machine.cpp)

include_directories(${CMAKE_SOURCE_DIR}/SSA)
include_directories(${CMAKE_SOURCE_DIR}/Lex)
# 声明模块的库文件
add_library(${MODULE_NAME} ${MODULE_SOURCES})
