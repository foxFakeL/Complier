#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "config.h"
using namespace std;

struct ITEM {
    size_t pid;   // 产生式编号
    int dot;      // 点位置，以产生式体最左端为0起
    string peek;  // 向前看符号

    bool operator<(const ITEM& it) const noexcept {
        if (this->pid != it.pid)
            return this->pid < it.pid;
        else if (this->dot != it.dot)
            return this->dot < it.dot;
        else if (this->peek != it.peek)
            return this->peek < it.peek;
        return false;
    }

    bool operator==(const ITEM& it) const noexcept {
        return (this->pid == it.pid) && (this->dot == it.dot) && (this->peek == it.peek);
    }
};

/*
 *	@brief	通过上下文无关文法初始化LR1语法分析器，同时也将CFG里面的语义规则获取，计算出了goto表和action表
 *	@param	path(string):	包含上下文无关文法文本的地址
 */
class SDD {
private:
    set<string> vt;                            // 终结符集
    set<string> vn;                            // 非终结符集
    map<string, set<string>> first;            // first集
    vector<vector<string>> prod;               // 产生式，[x][0]为产生式头
    vector<set<ITEM>> collection;              // LR(1)项集族
    map<pair<int, string>, int> trans;         // 项集转换函数
    map<pair<int, string>, int> action_table;  // ACTION表
    map<pair<int, string>, int> goto_table;    // GOTO表
    map<int, vector<string>> rules;            // 语义规则

public:
    SDD(string path);
    vector<string> get_pd(int i);
    vector<string> get_rules(int i);
    int ACTION(int i, string t);
    int GOTO(int i, string n);
    void print();

protected:
    void cal_first();
    void cal_collec();
    void cal_action();
    void cal_goto();
    void set_rule(int i, string rule);
    set<ITEM> closure(set<ITEM>& I);
    set<ITEM> go_to(set<ITEM>& I, string x);
    set<string> get_first(vector<string> v);
};
