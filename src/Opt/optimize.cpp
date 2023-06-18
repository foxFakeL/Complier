#include "optimize.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../SSA/AST.h"
#include "../SSA/Semer.h"

using namespace std;
// 对四元式序列进行常数折叠优化
void optimize_constant_folding(vector<QUAD*>& quads) {
    for (auto& quad : quads) {
        if (quad->op == "+" || quad->op == "-" || quad->op == "*" || quad->op == "/") {
            // 如果当前操作是加减乘除，则尝试进行常数折叠
            if (isdigit(quad->arg1[0]) && isdigit(quad->arg2[0])) {
                // 如果两个参数都是数字，则直接计算结果并替换四元式
                int num1 = stoi(quad->arg1);
                int num2 = stoi(quad->arg2);
                int result;
                if (quad->op == "+") {
                    result = num1 + num2;
                } else if (quad->op == "-") {
                    result = num1 - num2;
                } else if (quad->op == "*") {
                    result = num1 * num2;
                } else {
                    result = num1 / num2;
                }
                quad->op = "=";
                quad->arg1 = to_string(result);
                quad->arg2 = "";
            }
        }
    }
}
// 对四元式序列进行公共子表达式消除优化
void optimize_common_subexpression_elimination(vector<QUAD*>& quads) {
    unordered_map<string, string> subexpr_table;  // 子表达式表
    for (auto& quad : quads) {
        if (quad->op == "+" || quad->op == "-" || quad->op == "*" || quad->op == "/") {
            // 如果当前操作是加减乘除，则检查是否存在相同的子表达式
            string subexpr = "(" + quad->arg1 + quad->op + quad->arg2 + ")";
            if (subexpr_table.count(subexpr)) {
                // 如果存在相同的子表达式，则将该四元式替换为一个赋值操作
                quad->op = "=";
                quad->arg1 = subexpr_table[subexpr];
                quad->arg2 = "";
            } else {
                // 否则，将当前子表达式添加到子表达式表中
                subexpr_table[subexpr] = quad->res;
            }
        }
    }
}
// 对四元式序列进行死代码删除优化
void optimize_dead_code_elimination(vector<QUAD*>& quads) {
    unordered_set<string> live_vars;  // 活跃变量集合
    for (int i = quads.size() - 1; i >= 0; i--) {
        // 逆序遍历四元式序列，将所有结果变量添加到活跃变量集合中，并将对应的使用变量从集合中删除
        live_vars.insert(quads[i]->res);
        if (!quads[i]->arg1.empty()) {
            live_vars.erase(quads[i]->arg1);
        }
        if (!quads[i]->arg2.empty()) {
            live_vars.erase(quads[i]->arg2);
        }
        // 如果当前操作是赋值操作且结果变量不在活跃变量集合中，则将该四元式删除
        if (quads[i]->op == "=" && !live_vars.count(quads[i]->res)) {
            quads.erase(quads.begin() + i);
        }
    }
}
// 打印四元式序列
void print_quads(const vector<QUAD*>& quads) {
    for (const auto& quad : quads) {
        cout << quad->op << " " << quad->arg1 << " " << quad->arg2 << " " << quad->res << endl;
    }
}

void optimize(vector<QUAD*>& quads, Semer& semer) {
    // 对四元式序列进行常数折叠优化
    optimize_constant_folding(quads);

    // 对四元式序列进行公共子表达式消除优化
    optimize_common_subexpression_elimination(quads);

    // 对四元式序列进行死代码删除优化
    optimize_dead_code_elimination(quads);
}
