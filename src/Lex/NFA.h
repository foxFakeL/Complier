#pragma once
#define RG_INITIAL_SYM "BEGIN"
#include <map>
#include <set>
#include <string>

class NFA {
private:
    std::set<std::string> states;                                          // 状态集合
    std::set<char> inputs;                                                 // 输入符号集合
    std::map<std::pair<std::string, char>, std::set<std::string> > trans;  // 转换函数
    std::string is = RG_INITIAL_SYM;                                       // 开始状态
    std::set<std::string> fs;                                              // 接受状态

public:
    NFA(std::string path);
    std::string get_is();
    std::set<std::string> get_fs();
    std::set<char> get_inputs();
    std::set<std::string> move(std::set<std::string> T, char a);
    std::set<std::string> closure(std::set<std::string> T);
    void print();

protected:
    void set_edge(std::string s0, std::set<char> i, std::string s1);
};
