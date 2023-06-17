#pragma once

#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

class AST;
class AST_NODE;
class QUAD;
struct SYM {
    std::string type;
};
/*
 * @brief 语义分析，生成抽象语法树
 */
class Semer {
private:
    AST* ast;                                                    // 带注释的抽象语法树
    std::stack<std::map<std::string, std::list<SYM*>>*> sym_st;  // 符号表栈
    std::map<std::string, std::string> label_map;                // 标号映射表

    static int t_num;  // 中间变量寄存器计数
    static int l_num;  // 标号计数
    static int i_num;  // GOTO地址中间变量计数

public:
    Semer();
    AST* get_ast();
    AST_NODE* execute(std::vector<std::string> pd, std::vector<std::string> rules, std::stack<AST_NODE*>& nt);
    std::string get_true_label(std::string instr);
    void print();

protected:
    std::string get_attr(AST_NODE* node, std::string attr);
    std::vector<QUAD*> get_code(AST_NODE* node, std::string node_name);
    void set_addr(AST_NODE* node, std::string right);
    void set_true(AST_NODE* node, std::string right);
    void set_fals(AST_NODE* node, std::string right);
    void set_next(AST_NODE* node, std::string right);
    void set_code(AST_NODE* node, std::string right);
};