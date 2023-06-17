#include <fstream>
#include <iostream>

#include "Lex/DFA.h"
#include "Lex/Lexer.h"
#include "Lex/NFA.h"
#include "Lex/Token.h"
#include "SSA/AST.h"
#include "SSA/Parser.h"
#include "SSA/SDD.h"
#include "SSA/Semer.h"

#define FA_OUT 1
#define LR1_OUT 0
#define AST_OUT 1

using namespace std;

int main() {
    fstream tokens_ofile("result/Tokens.txt");  // 词法分析结果输出文件
    fstream ast_ofile("result/ast.txt");        // 抽象语法树的结构输出文件
    fstream LR1_ofile("result/LR1.txt");
    NFA nfa("assets/RG.txt");
    auto std_cout = cout.rdbuf();

    cout.rdbuf(tokens_ofile.rdbuf());
    if (FA_OUT)
        nfa.print();

    DFA dfa(&nfa);
    if (FA_OUT)
        dfa.print();

    Lexer lex("assets/code.txt", &dfa);
    if (lex.get_res() == 0)
        return 0;
    lex.print();

    cout.rdbuf(LR1_ofile.rdbuf());
    SDD sdd("assets/CFG.txt");
    if (LR1_OUT)
        sdd.print();

    Semer semer;
    Parser parser(lex.get_tokens(), sdd, semer);
    parser.print();
    cout.rdbuf(ast_ofile.rdbuf());
    AST* ast = semer.get_ast();
    AST_NODE* root = ast->get_root();

    if (root) {
        if (AST_OUT) {
            ast->print(ast->get_root());
            cout << endl;
        }
        cout.rdbuf(std_cout);
        cout << "[Quadruple]" << endl;
        for (auto quad : ast->get_root()->code) {
            quad->print(semer);
        }
    }

    return 0;
}