#include "Token.h"

using namespace std;

ostream& operator<<(ostream& os, const Token& token) {
    os << "<" << token.line << ", " << token.type << ", " << token.content << ">";
    return os;
}

bool Token::operator<(const Token& token) const {
    if (line < token.line)
        return true;
    else if (line == token.line) {
        if (type < token.type)
            return true;
        else if (type == token.type) {
            if (content < token.content)
                return true;
            else
                return false;
        } else
            return false;
    } else
        return false;
}