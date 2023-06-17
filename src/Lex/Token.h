#pragma once

#include <iostream>
#include <string>

struct Token {
    int line;             // 表示在第几行
    std::string type;     // 类型
    std::string content;  // 它的内容是什么
    bool isTerminal;      // 是否是终结符

    friend std::ostream& operator<<(std::ostream& os, const Token& token);

    bool operator<(const Token& token) const;
};