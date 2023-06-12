#pragma once

#include <map>
#include <string>
#include <vector>
using namespace std;

class QUAD {
    string op;  // 操作符
    string arg1;
    string arg2;
    string res;
};

// 抽象语法树节点
class AST_NODE {
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
public:
    AST_NODE* root;
    AST() : root{nullptr} {};
    void print();
};