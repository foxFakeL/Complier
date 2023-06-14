#pragma once

#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "AST.h"
using namespace std;

struct SYM {
    string type;
};
/*
 * @brief 语义分析，生成抽象语法树
 */
class Semer {
private:
    AST* ast;                                // 带注释的抽象语法树
    stack<map<string, list<SYM*>>*> sym_st;  // 符号表栈
    map<string, string> label_map;           // 标号映射表

    static int t_num;  // 中间变量寄存器计数
    static int l_num;  // 标号计数
    static int i_num;  // GOTO地址中间变量计数

public:
    Semer();
    AST* get_ast();
    AST_NODE* execute(vector<string> pd, vector<string> rules, stack<AST_NODE*>& nt);
    string get_true_label(string instr);
    void print();

protected:
    string get_attr(AST_NODE* node, string attr);
    vector<QUAD*> get_code(AST_NODE* node, string node_name);
    void set_addr(AST_NODE* node, string right);
    void set_true(AST_NODE* node, string right);
    void set_fals(AST_NODE* node, string right);
    void set_next(AST_NODE* node, string right);
    void set_code(AST_NODE* node, string right);
};