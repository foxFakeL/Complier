# 开发相关说明

## 项目结构

```
src
├── CMakeLists.txt
├── Lex                 // 词法分析
│   ├── CMakeLists.txt
│   ├── DFA.cpp
│   ├── DFA.h
│   ├── Lexer.cpp
│   ├── Lexer.h
│   ├── NFA.cpp
│   ├── NFA.h
│   ├── Token.cpp
│   └── Token.h
├── SSA                 // 语法和语义分析
│   ├── AST.cpp
│   ├── AST.h
│   ├── CMakeLists.txt
│   ├── Parser.cpp
│   ├── Parser.h
│   ├── Production.cpp
│   ├── Production.h
│   ├── SDD.cpp
│   ├── SDD.h
│   ├── SSA_main.cpp
│   ├── Semer.cpp
│   ├── Semer.h
│   └── config.h
├── VM                  // 用于运行中间代码的虚拟机
│   ├── CMakeLists.txt
│   ├── Machine.cpp
│   └── Machine.h
└── main.cpp
```

## 中间代码优化

- 原始

op|arg1|arg2|result
---|---|---|---
+|a|b|t1
+|a|b|t2

- 简化后

op|arg1|arg2|result
---|---|---|---
+|a|b|t1
=|t1||t2

