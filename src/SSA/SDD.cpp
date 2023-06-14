#include "SDD.h"

#include <fstream>
#include <iostream>
#include <regex>
using namespace std;

SDD::SDD(string path) {
    ifstream prod_file(path);
    if (!prod_file.is_open()) {
        cout << "cannot open CFG file!" << endl;
        return;
    }

    string tmp;
    regex e1("([^\\s]+)\\s*->\\s(.+)");    // 形如 A -> B 的式子，用于提取产生式头和体
    regex e2("([^\\s]+)\\s(.+)");          // 形如 B1 B2 的式子，用于提取产生式右侧
    regex e3("\\s*[{]\\s(.+)\\s[}]\\s*");  // 形如 { X } 的式子，用于提取属性文法
    smatch sm;

    int num_pd = 0;  // 当前产生式编号
    while (getline(prod_file, tmp)) {
        // 忽略注释行
        if (tmp[0] == '/' && tmp[1] == '/') {
            continue;
        }
        if (regex_search(tmp, sm, e1)) {
            vector<string> pd;        // 当前产生式
            pd.push_back(sm.str(1));  // 产生式头
            this->vn.insert(sm.str(1));

            tmp = sm.str(2);

            // 产生式体匹配
            while (regex_search(tmp, sm, e2)) {
                pd.push_back(sm.str(1));
                this->vt.insert(sm.str(1));
                tmp = sm.str(2);
            }
            pd.push_back(tmp);
            this->vt.insert(tmp);

            this->prod.push_back(pd);
            ++num_pd;
        } else if (regex_search(tmp, sm, e3)) {
            set_rule(num_pd, sm.str(1));
        }
    }
    prod_file.close();

    for (auto each : vn) {
        vt.erase(each);
    }

    cout << "cal FIRST ..." << endl;
    cal_first();
    cout << "cal COLLECTION ..." << endl;
    cal_collec();
    cout << "cal ACTION table ..." << endl;
    cal_action();
    cout << "cal GOTO table ..." << endl;
    cal_goto();
}

/*
 *	@brief	 计算各符号的FIRST集
 */
void SDD::cal_first() {
    for (auto each : vt) {
        first.insert({each, set<string>{each}});
    }
    first.insert({"#", set<string>{"#"}});

    bool flag = true;
    while (flag) {
        flag = false;
        for (auto each : prod) {
            vector<string> args(each.begin() + 1, each.end());
            set<string> ret = get_first(args);
            ret.insert(first[each[0]].begin(), first[each[0]].end());

            if (ret != first[each[0]]) {
                first[each[0]] = ret;
                flag = true;
            }
        }
    }
}

/*
 *	@brief	获取指定串的FIRST集
 */
set<string> SDD::get_first(vector<string> v) {
    set<string> ret;
    for (auto each : v) {
        ret.insert(first[each].begin(), first[each].end());
        if (first[each].find("#") == first[each].end()) {
            ret.erase("#");
            break;
        }
    }

    return ret;
}

/*
 *	@brief	返回项集I的闭包
 */
set<ITEM> SDD::closure(set<ITEM>& I) {
    set<ITEM> J = I;
    bool flag = true;

    while (flag) {
        flag = false;
        for (auto each : J) {
            vector<string> pd = prod[each.pid];  // 当前产生式

            if (size_t(each.dot + 1) < pd.size()) {
                string after_dot = pd[each.dot + 1];  // 点后符号
                // 点后的符号为非终结符
                if (vn.find(after_dot) != vn.end()) {
                    for (size_t i = 0; i < prod.size(); i++) {
                        // 对所有以点后符号开头的产生式
                        if (prod[i][0] == after_dot) {
                            // 计算first集合
                            vector<string> args;
                            for (size_t j = each.dot + 2; j < pd.size(); j++) {
                                args.push_back(pd[j]);
                            }
                            args.push_back(each.peek);

                            for (auto p : get_first(args)) {
                                ITEM item = {i, 0, p};
                                if (J.find(item) == J.end()) {
                                    J.insert(item);
                                    flag = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return J;
}

/*
 *	@brief	计算项集I经过输入x的转换
 */
set<ITEM> SDD::go_to(set<ITEM>& I, string x) {
    set<ITEM> J;
    for (auto each : I) {
        vector<string> pd = prod[each.pid];
        // 点后符号存在且点后符号为x
        if (size_t(each.dot + 1) < pd.size() && pd[each.dot + 1] == x) {
            ITEM item = {each.pid, each.dot + 1, each.peek};
            J.insert(item);
        }
    }
    return closure(J);
}

/*
 *	@brief	计算LR1项集族
 */
void SDD::cal_collec() {
    set<ITEM> I0 = {ITEM{0, 0, "#"}};
    collection.push_back(closure(I0));

    bool flag = true;
    while (flag) {
        flag = false;
        for (size_t i = 0; i < collection.size(); i++) {
            set<string> v = vn;
            v.insert(vt.begin(), vt.end());
            v.erase("#");
            for (auto sym : v) {
                set<ITEM> In = go_to(collection[i], sym);

                if (!In.empty()) {
                    // 项集是否已经在COLLECTION中
                    bool exist = false;
                    size_t j = 0;
                    for (; j < collection.size(); j++) {
                        if (collection[j] == In) {
                            exist = true;
                            break;
                        }
                    }
                    // 项集未存在
                    if (!exist) {
                        collection.push_back(In);
                        flag = true;
                    }

                    trans.insert({make_pair(i, sym), j});
                }
            }
        }
    }
}

/*
 *	@brief	计算ACTION表
 */
void SDD::cal_action() {
    for (size_t i = 0; i < collection.size(); i++) {
        for (auto e : collection[i]) {
            vector<string> pd = prod[e.pid];

            // 点在产生式最末 或 点后为空（规约或接受）
            if (size_t(e.dot + 1) == pd.size() || pd[e.dot + 1] == "#") {
                if (pd[0] != START_SYM) {
                    action_table[make_pair(i, e.peek)] = e.pid + 1;
                } else {
                    action_table[make_pair(i, e.peek)] = ACC;
                }
            }

            // 点后为非终结符（移入）
            else if (vt.find(pd[e.dot + 1]) != vt.end()) {
                string a = pd[e.dot + 1];
                pair<int, string> p(i, a);

                if (trans.find(p) != trans.end()) {
                    action_table[p] = -trans[p];
                }
            }
        }
    }
}

/*
 *	@brief	计算GOTO表
 */
void SDD::cal_goto() {
    for (auto i : trans) {
        string sym = i.first.second;
        if (vn.find(sym) != vn.end()) {
            goto_table[make_pair(i.first.first, sym)] = i.second;
        }
    }
}

/*
 *	@brief	获取编号为i的产生式的头（以第一个产生式编号为1）
 */
vector<string> SDD::get_pd(int i) {
    return prod[i - 1];
}

/*
 *	@brief	ACTION表对外接口
 *	@return ACC		接受
 *			>0		规约
 *			<=0		移入
 */
int SDD::ACTION(int i, string t) {
    if (action_table.find(make_pair(i, t)) != action_table.end()) {
        return action_table[make_pair(i, t)];
    }
    return ERROR;
}

/*
 *	@brief	GOTO表对外接口
 */
int SDD::GOTO(int i, string n) {
    return goto_table[make_pair(i, n)];
}

/*
 *	@brief  将语义规则绑定到产生式i
 */
void SDD::set_rule(int i, string rule) {
    if (rules.find(i) != rules.end()) {
        rules[i].push_back(rule);
    } else {
        vector<string> tmp;
        tmp.push_back(rule);
        rules.insert({i, tmp});
    }
}

/*
 *	@brief 返回产生式i的语义规则
 */
vector<string> SDD::get_rules(int i) {
    return rules[i];
}

/*
 *	@brief	打印SDD参数及生成的LR1语法分析表参数
 */
void SDD::print() {
    // 产生式及语义规则
    cout << "Productions: \n";
    for (size_t j = 0; j < prod.size(); j++) {
        auto each = prod[j];
        for (size_t i = 0; i < each.size(); i++) {
            if (i == 0)
                cout << each[i] << "\t-> ";
            else
                cout << each[i] << " ";
        }
        cout << endl;

        for (auto r : rules[j]) {
            cout << "\t" << r << endl;
        }
    }

    cout << "Vn: ";
    for (auto each : this->vn) {
        cout << each << "  ";
    }

    cout << endl
         << "Vt: ";
    for (auto each : this->vt) {
        cout << each << "  ";
    }

    cout << endl
         << "FIRST: " << endl;
    for (auto each : this->first) {
        cout << each.first << "\t: ";
        for (auto i : each.second) {
            cout << i << "  ";
        }
        cout << endl;
    }

    cout << endl
         << "COLLECTION: " << endl;
    for (size_t i = 0; i < collection.size(); i++) {
        cout << "I" << i << ":";
        for (auto each : collection[i]) {
            auto pd = prod[each.pid];
            for (size_t j = 0; j < pd.size(); j++) {
                if (j == 0)
                    cout << "\t" << pd[j] << "\t-> ";
                else
                    cout << pd[j] << " ";
                if (j == each.dot)
                    cout << "·";
            }
            cout << " , " << each.peek << endl;
        }
    }

    cout << endl
         << "TRANS" << endl;
    for (auto each : this->trans) {
        cout << "[" << each.first.first << ","
             << each.first.second << "] -> " << each.second << endl;
    }
    cout << endl;

    cout << endl
         << "[ACTION]" << endl;
    for (auto each : this->action_table) {
        cout << "[" << each.first.first << ","
             << each.first.second << "] -> " << each.second << endl;
    }
    cout << endl;
    cout << endl
         << "[GOTO]" << endl;
    for (auto each : this->goto_table) {
        cout << "[" << each.first.first << ","
             << each.first.second << "] -> " << each.second << endl;
    }
    cout << endl;
}