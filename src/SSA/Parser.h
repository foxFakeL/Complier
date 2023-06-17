#pragma once
#define START_SYM "PROGRAM"
#define STATE_STACK_OUT 1
#include <set>
#include <stack>
#include <vector>

class Token;
class AST_NODE;
class SDD;
class Semer;

/*
 * @brief 语法分析器，判断是否通过语法分析，同时将规则附带的语义传入Semer中，由Semer进行语义分析
 * @param tokens 待分析的token数组
 * @param sdd 包含语法语义规则的类
 * @param semer 语义分析器
 */
class Parser {
private:
    std::stack<int> st;        // 状态栈
    std::stack<AST_NODE*> nt;  // 节点栈
    int res = 0;               // 处理结果

public:
    Parser(const std::vector<Token>& tokens, SDD& sdd, Semer& semer);
    void print();

protected:
};