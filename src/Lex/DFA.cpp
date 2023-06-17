#include "DFA.h"

#include <iostream>
#include <stack>
#include <vector>

#include "NFA.h"
using namespace std;

/*
 *	@brief	通过NFA构造DFA
 *	@param	nfa(NFA*):	传入的NFA指针
 */
DFA::DFA(NFA* nfa) {
    int st_num = 0;                                               // DFA状态编号
    map<set<string>, int> dfa_states;                             // NFA->DFA状态映射
    stack<set<string>> flags;                                     // DFA状态标记
    set<string> s0 = nfa->closure(set<string>({nfa->get_is()}));  // DFA初态
    this->inputs = nfa->get_inputs();
    this->inputs.erase('#');

    dfa_states.insert({nfa->closure(s0), st_num++});
    flags.push(s0);

    while (!flags.empty()) {
        set<string> T = flags.top();
        set<char> inputs = nfa->get_inputs();
        flags.pop();

        int l = dfa_states[T];  // NFA状态T对应的DFA编号
        for (auto a : inputs) {
            set<string> next_states = nfa->move(T, a);
            if (next_states.empty()) {
                continue;
            }
            set<string> N = nfa->closure(next_states);
            auto search = dfa_states.find(N);
            if (search == dfa_states.end()) {
                dfa_states.insert({N, st_num++});
                flags.push(N);
            }

            // 建立边
            int r = dfa_states[N];
            this->trans.insert({make_pair(l, a), r});
        }

        // 检查是否为接受状态
        for (auto each : nfa->get_fs()) {
            if (T.find(each) != T.end()) {
                this->fs.insert(l);
                this->token.insert({l, each});
            }
        }

        // 更新状态集
        this->states.insert(l);
    }
}

/*
 *	@brief	返回由状态T经输入a到达的状态
 */
int DFA::move(int T, char a) {
    if (this->trans.find(make_pair(T, a)) != this->trans.end()) {
        return this->trans[make_pair(T, a)];
    }
    return -1;
}

/*
 *	@brief	判断是否为接受状态
 */
bool DFA::is_fs(int T) {
    if (this->fs.find(T) != this->fs.end())
        return true;
    return false;
}

/*
 *	@brief	返回token
 */
string DFA::get_token(int T) {
    return this->token[T];
}

/*
 *	@brief	打印DFA参数
 */
void DFA::print() {
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
         << "Token map:" << endl;
    for (auto each : this->token) {
        cout << each.first << " - " << each.second << endl;
    }
    cout << "Inputs: ";
    for (auto each : this->inputs) {
        cout << each << "  ";
    }
    cout << endl
         << "Transitions: " << endl;
    for (const auto& each : this->trans) {
        cout << "[" << each.first.first << "," << each.first.second << "] -> " << each.second << endl;
    }
    cout << endl;
}
