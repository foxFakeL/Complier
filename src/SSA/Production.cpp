#include "Production.h"

Production::Production(string lhs, vector<string> rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

ostream& operator<<(ostream& os, const Production& p) {
    os << p.lhs << " -> ";
    for (auto i : p.rhs) {
        os << i << " ";
    }
    return os;
}

string& Production::operator[](int i) const noexcept {
    if (i == 0) {
        return const_cast<string&>(this->lhs);
    } else {
        return const_cast<string&>(this->rhs[i - 1]);
    }
}