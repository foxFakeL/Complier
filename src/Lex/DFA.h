#pragma once

#include <map>
#include <set>
#include <string>

class NFA;

class DFA {
private:
    std::set<int> states;                       // 状态集合
    std::set<char> inputs;                      // 输入符号集合
    std::map<std::pair<int, char>, int> trans;  // 转换函数
    int is = 0;                                 // 开始状态
    std::set<int> fs;                           // 接受状态
    std::map<int, std::string> token;           // 接受状态token映射

public:
    DFA(NFA* nfa);
    int move(int T, char a);
    bool is_fs(int T);
    std::string get_token(int T);
    void print();
};
