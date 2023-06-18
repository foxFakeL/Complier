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
#include "VM/Machine.h"

#define FA_OUT 0    // 是否输出NFA和DFA，文件
#define LR1_OUT 0   // 是否输出LR1分析表，文件
#define AST_OUT 1   // 是否输出抽象语法树，文件
#define QUAD_OUT 1  // 是否输出四元式, 终端

using namespace std;

int main(int arg, char** args) {
    fstream tokens_ofile("result/Tokens.txt", ios::out);  // 词法分析结果输出文件
    fstream ast_ofile("result/ast.txt", ios::out);        // 抽象语法树的结构输出文件
    fstream LR1_ofile("result/LR1.txt", ios::out);
    NFA nfa("assets/RG.txt");
    auto std_cout = cout.rdbuf();

    cout.rdbuf(tokens_ofile.rdbuf());
    if (FA_OUT)
        nfa.print();

    DFA dfa(&nfa);
    if (FA_OUT)
        dfa.print();
    string code_file_path = "assets/code.txt";
    if (arg == 2) {
        code_file_path = args[1];
    }
    Lexer lex(code_file_path, &dfa);
    if (lex.get_res() == 0) {
        cout.rdbuf(std_cout);
        cout << "Lexical Error: " << lex.lex_res.msg
             << " \nat line " << lex.lex_res.line << endl;
        return 0;
    }
    lex.print();
    tokens_ofile.close();

    cout.rdbuf(LR1_ofile.rdbuf());
    SDD sdd("assets/CFG.txt");
    if (LR1_OUT)
        sdd.print();

    Semer semer;
    Parser parser(lex.get_tokens(), sdd, semer);
    auto res = parser.parse();
    if (res.res == 0) {
        cout.rdbuf(std_cout);
        cout << "Syntax Error: " << res.msg << " \nat line " << res.line << endl;
        return 0;
    }
    parser.print();
    LR1_ofile.close();

    cout.rdbuf(ast_ofile.rdbuf());
    AST* ast = semer.get_ast();
    AST_NODE* root = ast->get_root();

    if (root && QUAD_OUT) {
        if (AST_OUT) {
            ast->print(ast->get_root());
            cout << endl;
        }
        ast_ofile.close();
        cout.rdbuf(std_cout);
        cout << "[Quadruple]" << endl;
        // for (auto quad : ast->get_root()->code) {
        //     cout << *quad << endl;
        // }
        // cout << "******************************" << endl;
        for (const auto& i : ast->get_root()->code) {
            i->print(semer);
        }
    }
    ast_ofile.close();
    cout.rdbuf(std_cout);
    VirtualMation(ast->get_root()->code, semer);

    // cout.rdbuf(std_cout);
    // string s;
    // cin >> s;
    // if (isDegital(s)) {
    //     cout << "aaaaaaaaaaaaaaaaaaaaaa\n";
    // } else {
    //     cout << "bbbbbbbbbbbbbbbbbbbbbbbbbb\n";
    // }
    // return 0;
}