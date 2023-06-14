#include <fstream>
#include <iostream>

#include "Parser.h"
#include "Production.h"
#include "SDD.h"

using namespace std;

vector<string> split(const string& str, const string& delim) {
    vector<string> res;
    if ("" == str)
        return res;
    char* strs = new char[str.length() + 1];
    strcpy(strs, str.c_str());
    char* d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());
    char* p = strtok(strs, d);
    while (p) {
        string s = p;
        res.push_back(s);
        p = strtok(NULL, d);
    }
    return res;
}

int main() {
    vector<Token> tokens;
    fstream token_file("../assets/Tokens.txt");
    string line;
    while (getline(token_file, line)) {
        Token token;
        line = line.substr(1, line.length() - 2);
        vector<string> token_info = split(line, ", ");
        token.line = stoi(token_info[0]);
        token.type = token_info[1];
        token.content = token_info[2];
        tokens.emplace_back(token);
        // cout << token << endl;
    }
    Semer semer;
    SDD sdd("../assets/CFG.txt");
    Parser praser = Parser(tokens, sdd, semer);
    AST* ast = semer.get_ast();
    AST_NODE* root = ast->get_root();

    // cout << "AST:" << endl;

    if (root != nullptr) {
        for (const auto& quad : root->code) {
            cout << *quad << "\n";
            // quad->print(semer);
        }
        ast->print(root);
    }
    return 0;
}