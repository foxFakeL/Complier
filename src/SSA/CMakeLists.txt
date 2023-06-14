# 声明模块名称和源文件列表
set(MODULE_NAME SSA)
set(MODULE_SOURCES Production.cpp Production.h Parser.cpp
    Parser.h AST.h AST.cpp Semer.h Semer.cpp SDD.h SDD.cpp)

include_directories(${CMAKE_SOURCE_DIR}/Lex)

# 声明模块的库文件
add_library(${MODULE_NAME} ${MODULE_SOURCES})

# 添加可执行文件
add_executable(SSA_test SSA_main.cpp ${MODULE_SOURCES})