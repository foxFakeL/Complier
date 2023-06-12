# 添加 SSA 模块的子目录
add_subdirectory(SSA)

# 添加可执行文件
add_executable(Complier main.cpp)

# 设置可执行文件输出目录
set(OUTPUT_PATH ${CMAKE_BINARY_DIR})

# 将资源文件复制到可执行文件输出目录
configure_file(../assets/CFG.txt ${OUTPUT_PATH}/assets/CFG.txt COPYONLY)
configure_file(../assets/Tokens.txt ${OUTPUT_PATH}/assets/Tokens.txt COPYONLY)

# 链接可执行文件和 SSA 模块的库文件
target_link_libraries(Complier SSA)