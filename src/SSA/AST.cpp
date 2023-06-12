#include "AST.h"

#include <iostream>

#include "Semer.h"
using namespace std;

void QUAD::print(Semer& semer) {
    if (op == "goto") {
        string true_label = semer.get_true_label(res);
        cout << "\t" << op << " " << true_label << endl;
    } else if (op == ">=" || op == "<=" || op == "<" || op == ">" || op == "==" || op == "!=") {
        string true_label = semer.get_true_label(res);
        cout << "\tif " << arg1 << " " << op << " " << arg2 << " goto " << true_label << endl;
    } else if (op == "=") {
        cout << "\t" << res << " " << op << " " << arg1 << endl;
    } else if (op == "label") {
        cout << res << ":";
    } else if (!res.empty() && !arg1.empty() && !arg2.empty()) {
        cout << "\t" << res << " = " << arg1 << " " << op << " " << arg2 << endl;
    } else {
        cout << "OP: " << op << "\tArg1: " << arg1 << "\tArg2: " << arg2 << "\tRes: " << res << endl;
    }
}

void AST::set_root(AST_NODE* root) {
    this->root = root;
}

AST_NODE* AST::get_root() {
    return this->root;
}

void AST::print(AST_NODE* n, int deep) {
    int tmp = deep;
    while (tmp--) cout << "    ";
    cout << " |- " << n->val;
    if (!n->children.empty()) {
        for (auto c : n->children) {
            cout << endl;
            this->print(c.second, deep + 1);
            // cout << " (" << c.first << ") ";
        }
    }
}