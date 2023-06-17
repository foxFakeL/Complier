#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "config.h"

struct ITEM {
    size_t pid;        // 产生式编号
    int dot;           // 点位置，以产生式体最左端为0起
    std::string peek;  // 向前看符号

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
    std::set<std::string> vt;                                 // 终结符集
    std::set<std::string> vn;                                 // 非终结符集
    std::map<std::string, std::set<std::string>> first;       // first集
    std::vector<std::vector<std::string>> prod;               // 产生式，[x][0]为产生式头
    std::vector<std::set<ITEM>> collection;                   // LR(1)项集族
    std::map<std::pair<int, std::string>, int> trans;         // 项集转换函数
    std::map<std::pair<int, std::string>, int> action_table;  // ACTION表
    std::map<std::pair<int, std::string>, int> goto_table;    // GOTO表
    std::map<int, std::vector<std::string>> rules;            // 语义规则

public:
    SDD(std::string path);
    std::vector<std::string> get_pd(int i);
    std::vector<std::string> get_rules(int i);
    int ACTION(int i, std::string t);
    int GOTO(int i, std::string n);
    void print();

protected:
    void cal_first();
    void cal_collec();
    void cal_action();
    void cal_goto();
    void set_rule(int i, std::string rule);
    std::set<ITEM> closure(std::set<ITEM>& I);
    std::set<ITEM> go_to(std::set<ITEM>& I, std::string x);
    std::set<std::string> get_first(std::vector<std::string> v);
};
