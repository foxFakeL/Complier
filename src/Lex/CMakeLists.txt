# 声明模块名称和源文件列表
set(MODULE_NAME Lex)
set(MODULE_SOURCES Token.h Token.cpp Lexer.h Lexer.cpp DFA.h DFA.cpp NFA.cpp NFA.h)

# 声明模块的库文件
add_library(${MODULE_NAME} ${MODULE_SOURCES})

