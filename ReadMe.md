# 开发相关说明

## 项目结构

```
├── ReadMe.md
├── assets
│   ├── CFG.txt
│   ├── Tokens.txt
│   └── Trans.txt
└── src
    ├── CMakeLists.txt
    ├── Code_Generation     // 代码生成
    ├── Code_Optimization   // 代码优化
    ├── Lex                 // 词法分析
    │   ├── CMakeLists.txt
    │   └── Token.h
    ├── SSA                 // 语法语义分析
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
    └── main.cpp
```