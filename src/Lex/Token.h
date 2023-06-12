#pragma once

#include <iostream>
#include <string>

using namespace std;

struct Token {
    int line;         // 表示在第几行
    string type;      // 类型
    string content;   // 它的内容是什么
    bool isTerminal;  // 是否是终结符

    friend ostream& operator<<(ostream& os, const Token& token) {
        os << "<" << token.line << ", " << token.type << ", " << token.content << ">";
        return os;
    }

    bool operator<(const Token& token) const {
        if (line < token.line)
            return true;
        else if (line == token.line) {
            if (type < token.type)
                return true;
            else if (type == token.type) {
                if (content < token.content)
                    return true;
                else
                    return false;
            } else
                return false;
        } else
            return false;
    }
};