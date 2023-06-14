#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

class Semer;
// 四元式
struct QUAD {
    string op;
    string arg1;
    string arg2;
    string res;

    void print(Semer& semer);

    friend ostream& operator<<(ostream& os, const QUAD& q);
};

struct AST_NODE {
    string val;                       // 叶子节点：值；内部节点：代表的文法符号
    string addr;                      // 存储值的地址
    string truelb;                    // 为真时，转向的地址标号
    string falselb;                   // 为假时，转向的地址标号
    string nextlb;                    // 下一个语句的地址标号
    vector<QUAD*> code;               // 四元式代码序列
    map<string, AST_NODE*> children;  // 子节点，键为文法符号
};
// 抽象语法树
class AST {
private:
    AST_NODE* root;  // 抽象语法树根节点

public:
    void set_root(AST_NODE* root);
    AST_NODE* get_root();
    void print(AST_NODE* n, int deep = 0);
};
