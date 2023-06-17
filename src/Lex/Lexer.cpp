#include "Lexer.h"

#include <fstream>
#include <iostream>

#include "DFA.h"
#include "Token.h"
using namespace std;

Lexer::Lexer(string path, DFA* dfa) {
    ifstream code_file(path);
    if (!code_file.is_open()) {
        cout << "cannot open code file!";
        return;
    }

    string tmp;
    while (getline(code_file, tmp)) {
        if (!res)
            break;

        lexeme_begin = &tmp[0];
        forward = lexeme_begin;
        potential_lex = lexeme_begin;

        while (true) {
            if (*forward == ' ' || *forward == '\t' || *forward == '\n' || *forward == '\0') {
                // 当前状态可为接受状态
                if (state != 0) {
                    if (dfa->is_fs(state)) {
                        // 根据最长匹配规则，检测预读字符是否能推进状态机
                        /*if (dfa->move(state, peek) != -1) {
                            potential_lex = forward++;
                        }*/

                        // 接受词素
                        {
                            string content(lexeme_begin, forward);
                            string token_type = dfa->get_token(state);
                            Token* token;

                            // 关键字识别
                            if (token_type == "ID" && keywords.find(content) != keywords.end()) {
                                token = new Token{line, "KEYWORD", content};
                            } else {
                                token = new Token{line, token_type, content};
                            }

                            tokens.push_back(token);

                            if (*forward == '\0') {
                                state = 0;
                                break;
                            }

                            lexeme_begin = ++forward;
                            potential_lex = lexeme_begin;
                            state = 0;
                        }
                    } else {
                        res = 0;
                        cout << "Lexical error at line: " << line << endl;
                        string content(lexeme_begin, forward);
                        cout << "Incorrect lexeme: " << content << endl
                             << endl;
                        break;
                    }
                } else if (*forward == '\0')
                    break;
                else {
                    ++forward;
                    ++lexeme_begin;
                    ++potential_lex;
                    continue;
                }
            }

            int next_state = dfa->move(state, *forward);  // 下一状态
            char peek = *(forward + 1);                   // 预读字符

            // 检测当前字符能否推进状态机
            if (next_state != -1) {
                state = next_state;
                forward++;
            } else {
                // 错误处理
                res = 0;
                cout << "Lexical error at line: " << line << endl;
                string content(lexeme_begin, forward);
                cout << "Incorrect words after lexeme: " << content << endl
                     << endl;
                break;
            }
        }
        ++line;
    }

    Token* token = new Token{line, "#", "#"};
    tokens.push_back(token);
}

vector<Token> Lexer::get_tokens() {
    vector<Token> ret;
    for (const auto& i : tokens) {
        ret.emplace_back(*i);
    }
    return ret;
}

int Lexer::get_res() {
    return this->res;
}

void Lexer::print() {
    cout << endl
         << "[Token]" << endl;
    for (auto each : this->tokens) {
        cout << "<" << each->line << ", " << each->type << ", " << each->content << ">" << endl;
    }
    cout << endl;
}