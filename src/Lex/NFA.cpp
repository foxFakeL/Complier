#include "NFA.h"

#include <fstream>
#include <iostream>
#include <regex>
using namespace std;

/*
 *	@brief	通过正则文法构造NFA
 *	@param	path(string):	包含正则文法文本的地址
 */
NFA::NFA(string path) {
    ifstream prod_file(path);
    if (!prod_file.is_open()) {
        cout << "cannot open RG file!" << endl;
        return;
    }

    string tmp;
    regex e("([^\\s]+)\\s*->\\s([^\\s]+)\\s?([^\\s]*)");
    smatch sm;

    while (getline(prod_file, tmp)) {
        // 忽略注释行
        if (tmp[0] == '/' && tmp[1] == '/') {
            continue;
        }
        if (regex_search(tmp, sm, e)) {
            string input = sm.str(2);  // 输入符号a

            // 形如A->a
            string s1;
            if (sm.str(3).empty()) {
                s1 = "END";
                this->fs.insert(sm.str(1));
            }
            // 形如A->aB
            else {
                s1 = sm.str(3);
            }

            set<char> in_set;
            // 单字符
            if (input.size() == 1) {
                in_set = {input[0]};
                this->inputs.insert(input[0]);
            }
            // 元字符[0-9]
            else if (input == "[0-9]") {
                for (int i = 0; i < 10; i++) {
                    char c = '0' + i;
                    in_set.insert(c);
                    this->inputs.insert(c);
                }
            }
            // 元字符[a-z]
            else if (input == "[a-z]") {
                for (int i = 0; i < 26; i++) {
                    char c = 'a' + i;
                    in_set.insert(c);
                    this->inputs.insert(c);
                }
            }
            // 元字符[A-Z]
            else if (input == "[A-Z]") {
                for (int i = 0; i < 26; i++) {
                    char c = 'A' + i;
                    in_set.insert(c);
                    this->inputs.insert(c);
                }
            }

            this->states.insert(sm.str(1));  // 更新状态集
            this->set_edge(sm.str(1), in_set, s1);
        }
    }

    prod_file.close();
}

/*
 *	@brief	建立边
 */
void NFA::set_edge(string s0, set<char> i, string s1) {
    for (auto each : i) {
        pair<string, char> p(s0, each);
        auto search = this->trans.find(p);
        if (search != this->trans.end()) {
            this->trans[p].insert(s1);
        } else {
            this->trans.insert({p, set<string>{s1}});
        }
    }
}

/*
 *	@brief	返回NFA初始状态
 */
string NFA::get_is() {
    return this->is;
}

/*
 *	@brief	返回NFA接受状态
 */
set<string> NFA::get_fs() {
    return this->fs;
}

/*
 *	@brief	返回NFA输入符号集
 */
set<char> NFA::get_inputs() {
    return this->inputs;
}

/*
 *	@brief	返回从集合T中的状态经输入a能到达所有状态的集合
 */
set<string> NFA::move(set<string> T, char a) {
    set<string> ret;
    for (auto each : T) {
        set<string> tmp = this->trans[make_pair(each, a)];
        ret.insert(tmp.begin(), tmp.end());
    }
    return ret;
}

/*
 *	@brief	返回状态集合T的闭包
 */
set<string> NFA::closure(set<string> T) {
    set<string> ret;
    for (auto each : T) {
        ret.insert(each);
        set<string> t = trans[make_pair(each, '#')];
        ret.insert(t.begin(), t.end());

        t = closure(t);
        ret.insert(t.begin(), t.end());
    }
    return ret;
}

/*
 *	@brief	打印NFA参数
 */
void NFA::print() {
    cout << "States: ";
    for (auto each : this->states) {
        cout << each << "  ";
    }
    cout << endl
         << "Initial state: " << this->is << endl;
    cout << "Final states: ";
    for (auto each : this->fs) {
        cout << each << "  ";
    }
    cout << endl
         << "Inputs: ";
    for (auto each : this->inputs) {
        cout << each << "  ";
    }
    cout << endl
         << "Transitions: " << endl;
    for (auto each : this->trans) {
        cout << "[" << each.first.first << "," << each.first.second << "] -> (";
        for (auto it : each.second) {
            cout << it << ",";
        }
        cout << ")" << endl;
    }
    cout << endl;
}
