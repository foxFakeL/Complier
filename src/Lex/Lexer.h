#include <set>
#include <string>
#include <vector>
class DFA;
class Token;

class LexError {
public:
    bool res;
    int line;
    std::string msg;
};

class Lexer {
private:
    char* lexeme_begin = nullptr;   // 词素头
    char* forward = nullptr;        // 当前字符
    char* potential_lex = nullptr;  // 潜在接受位置
    int state = 0;                  // 当前状态
    int line = 1;                   // 当前行
    int res = 1;                    // 分析结果
    std::vector<Token*> tokens;     // token列表

public:
    LexError lex_res;
    Lexer(std::string path, DFA* dfa);
    std::vector<Token> get_tokens();
    int get_res();
    void print();
};

const std::set<std::string> keywords{"int", "float", "bool", "if", "else", "while", "do", "true", "false"};
