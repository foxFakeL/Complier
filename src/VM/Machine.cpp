#include "Machine.h"

#include <map>
#include <regex>
#include <string>

#include "../SSA/AST.h"
#include "../SSA/Semer.h"

#define PRINT 0  // 是否输出虚拟机执行过程

using namespace std;
// 判断是否为数字
bool isDegital(string str) {
    regex r("^[-+]?[0-9]+(.[0-9]+)?(e[-+]?[0-9]+)?$");
    smatch sm;
    return regex_match(str, sm, r);
}

int get_next(vector<QUAD*> quad, string label) {
    for (int i = 0; i < quad.size(); i++) {
        if (label == quad[i]->res) {  // 如果等于
            return i;                 // 则跳转到四元式执行
        }
    }
    return -1;
}
map<string, double> key_val;

bool compare(string op, float a, float b) {
    if (op == ">=") {
        return a >= b;
    } else if (op == "<=") {
        return a <= b;
    } else if (op == "==") {
        return a == b;
    } else if (op == "!=") {
        return a != b;
    } else if (op == ">") {
        return a > b;
    } else if (op == "<") {
        return a < b;
    }
    return false;
}

int Jump(vector<QUAD*> quad, string op, string arg1, string arg2, string true_label) {
    // 先要判断两个参数是否为数字
    if (isDegital(arg1)) {
        if (isDegital(arg2)) {                                             // 如果两个参数都为数字，则直接比较
            bool f = compare(op, atof(arg1.c_str()), atof(arg2.c_str()));  // 如果为真，则跳转
            if (f) {
                return get_next(quad, true_label);  // 获取跳转位置的索引
            } else {
                return -1;
            }
        } else {  // 参数1是数字，参数2不是数字
            // 将参数2作为变量取出
            double v2 = key_val[arg2];
            bool f = compare(op, atof(arg1.c_str()), v2);  // 如果为真，则跳转
            if (f) {
                return get_next(quad, true_label);  // 获取跳转位置的索引
            } else {
                return -1;
            }
        }
    } else {  // 参数1不是数字
        // 将参数1作为变量取出
        double v1 = key_val[arg1];
        if (isDegital(arg2)) {                             // 如果参数2是数字
            bool f = compare(op, v1, atof(arg2.c_str()));  // 如果为真，则跳转
            if (f) {
                return get_next(quad, true_label);  // 获取跳转位置的索引
            } else {
                return -1;
            }
        } else {  // 参数2不是数字
            // 将参数2作为变量取出
            double v2 = key_val[arg2];
            bool f = compare(op, v1, v2);
            if (f) {
                return get_next(quad, true_label);  // 获取跳转位置的索引
            } else {
                return -1;
            }
        }
    }
}
double calc(string op, double arg1, double arg2) {
    if (op == "+") {
        return arg1 + arg2;
    } else if (op == "-") {
        return arg1 - arg2;
    } else if (op == "*") {
        return arg1 * arg2;

    } else if (op == "/") {
        return arg1 / arg2;
    }
    return -1;
}
void handle(string op, string arg1, string arg2, string res) {
    // 先要判断两个参数是否为数字
    if (isDegital(arg1)) {
        if (isDegital(arg2)) {                                            // 如果两个参数都为数字，则直接比较
            double f = calc(op, atof(arg1.c_str()), atof(arg2.c_str()));  // 如果为真，则跳转
            key_val[res] = f;

        } else {  // 参数1是数字，参数2不是数字
            // 将参数2作为变量取出
            double v2 = key_val[arg2];
            double f = calc(op, atof(arg1.c_str()), v2);  // 如果为真，则跳转
            key_val[res] = f;
        }
    } else {  // 参数1不是数字
        // 将参数1作为变量取出
        double v1 = key_val[arg1];
        if (isDegital(arg2)) {                            // 如果参数2是数字
            double f = calc(op, v1, atof(arg2.c_str()));  // 如果为真，则跳转
            key_val[res] = f;
        } else {  // 参数2不是数字
            // 将参数2作为变量取出
            double v2 = key_val[arg2];
            double f = calc(op, v1, v2);
            key_val[res] = f;
        }
    }
}

void VirtualMation(vector<QUAD*> quad, Semer& semer) {
    int cur_index = 0;
    while (cur_index < quad.size()) {
        QUAD* q = quad[cur_index];  // 取出当前四元式执行相应的操作
        QUAD tq = *q;
        if (q->op == "=") {  // 如果为赋值符号，则直接赋值填入map中，没有则创建，存在则覆盖原值
            if (isDegital(q->arg1)) {
                key_val[q->res] = atof(q->arg1.c_str());
            } else {
                key_val[q->res] = key_val[q->arg1];
            }
        } else if (q->op == "goto") {                          // 跳转操作
            string true_label = semer.get_true_label(q->res);  // 获取真实跳转的位置
            cur_index = get_next(quad, true_label);            // 获取跳转位置的索引
            if (cur_index == -1) {
                cout << "=========================\n";
                for (const auto& p : key_val) {
                    cout << p.first << " : " << p.second << endl;
                }
                cout << "==========================\n";
                return;
            }
        } else if (q->op == ">=" || q->op == "<=" || q->op == "<" || q->op == ">" || q->op == "==" || q->op == "!=") {
            string true_label = semer.get_true_label(q->res);  // 获取真实跳转的位置
            int ret = Jump(quad, q->op, q->arg1, q->arg2, true_label);
            if (ret != -1)
                cur_index = ret;
        } else if (q->op == "+" || q->op == "-" || q->op == "*" || q->op == "/") {
            handle(q->op, q->arg1, q->arg2, q->res);
        }
        cur_index++;
        if (PRINT) {
            for (const auto& p : key_val) {
                cout << p.first << " : " << p.second << endl;
            }
        }
    }
    cout << "=========================\n";
    for (const auto& p : key_val) {
        cout << p.first << " : " << p.second << endl;
    }
    cout << "==========================\n";
}