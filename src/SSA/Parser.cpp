#include "Parser.h"

#include <iostream>
using namespace std;

Parser::Parser(const vector<Token>& tokens, SDD& sdd, Semer& semer) {
    st.push(0);
    for (size_t i = 0; i < tokens.size();) {
        auto token = &tokens[i];
        string inputs;
        if (token->type == "ID")
            inputs = "ID";
        else if (token->type == "INT" || token->type == "FLOAT" || token->type == "E_NUM" || token->type == "I_NUMF")
            inputs = "NUM";
        else
            inputs = token->content;

        int op = sdd.ACTION(st.top(), inputs);

        if (STATE_STACK_OUT) {
            cout << "INPUT: " << inputs << "\t";
            cout << "STACK TOP: " << st.top() << "\t";
        }

        // 报错
        if (op == ERROR) {
            res = 0;
            cout << "Syntax error at line: " << token->line << endl;
            cout << "Incorrect syntax: " << token->content << endl
                 << endl;
            break;
        }

        // 接受
        if (op == ACC) {
            res = 1;
            if (STATE_STACK_OUT)
                cout << "accept" << endl;
            break;
        }

        // 移入
        if (op <= 0) {
            AST_NODE* leaf = new AST_NODE();
            leaf->val = token->content;
            nt.push(leaf);

            st.push(-op);
            ++i;

            if (STATE_STACK_OUT)
                cout << "shift " << -op << endl;
        }

        // 规约
        if (op > 0) {
            vector<string> pd = sdd.get_pd(op);

            // 将产生式体数量的状态弹出栈
            for (size_t j = 1; j < pd.size(); j++) {
                if (st.size() > 1 && pd[j] != "#")
                    st.pop();
            }
            // 新状态压栈
            st.push(sdd.GOTO(st.top(), pd[0]));

            AST_NODE* node = semer.execute(pd, sdd.get_rules(op), nt);
            nt.push(node);

            if (STATE_STACK_OUT)
                cout << "reduce " << op << " -> " << pd[0] << endl;
        }
    }
}

void Parser::print() {
    cout << endl
         << "[Syntax Analysis Result]" << endl;
    if (res)
        cout << "YES" << endl;
    else
        cout << "NO" << endl;
    cout << endl;
}