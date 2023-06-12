#pragma once
#define ACC 9999
#define ERROR 8888
#define START_SYM "PROGRAM"
#define STATE_STACK_OUT 1
#include <set>
#include <stack>

#include "../Lex/Token.h"
#include "Production.h"
#include "Semer.h"

class Item {
public:
    int prod_id;            // 产生式的编号
    int dot_pos;            // 点的位置
    set<string> lookahead;  // 向前看符号
    bool operator<(const Item& item) const;
    bool operator==(const Item& item) const;
};

class Praser {
public:
    vector<Token> tokens;
    vector<Production> productions;
    set<string> terminals, nonterminals;
    map<string, set<string>> first;
    vector<set<Item>> item_collection;   // LR1项目集规范族
    map<pair<int, string>, int> trans;   // 状态转移表
    map<pair<int, string>, int> action;  // action表, 规约为正数，移入为负数
    map<pair<int, string>, int> goto_;   // goto表
    /**
     * @brief 构造函数
     * @param tokens 词法分析结果
     * @param filename CFG文件路径
     * @param semer 语义分析器
     */
    Praser(const vector<Token>& tokens, string filename, Semer& semer);

    /**
     * @brief 对外的GoTo表接口
     * @param state 状态
     * @param symbol 符号
     */
    int GOTO(int state, string symbol);
    /**
     * @brief 对外的Action表接口
     * @param state 状态
     * @param symbol 符号
     */
    int ACTION(int state, string symbol);

private:
    // 打印项目集规范族
    void print_collection();
    // 打印状态转移表
    void print_trans();
    void print_first();

    bool res;  // 语法分析结果
    // 生成 first 集
    void build_first();
    // 构建项目集规范族
    void build_collection();
    // 获得闭包项
    set<Item> get_closure(const Item& items);
    // 获得项目集经过某个符号的转移
    set<Item> get_goto(const set<Item>& items, const string& symbol);
    // 获取一个串的First集
    set<string> get_first(const vector<string>& vec);
    // 构建Action表
    void build_action();
    // 构建GoTo表
    void build_goto();
};
