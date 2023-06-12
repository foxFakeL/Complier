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

MultiProduction::MultiProduction(string lhs, vector<vector<string>> rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

ostream& operator<<(ostream& os, const MultiProduction& p) {
    os << p.lhs << " -> ";
    for (int i = 0; i < p.rhs.size(); i++) {
        for (int j = 0; j < p.rhs[i].size(); j++) {
            os << p.rhs[i][j] << " ";
        }
        if (i != p.rhs.size() - 1) {
            os << "| ";
        }
    }
    return os;
}

vector<Production> getProductions(string fileName) {
    regex e1("([^\\s]+)\\s*->\\s(.+)");    // 形如 A -> B 的式子，用于提取产生式头和体
    regex e2("([^\\s]+)\\s(.+)");          // 形如 B1 B2 的式子，用于提取产生式右侧
    regex e3("\\s*[{]\\s(.+)\\s[}]\\s*");  // 形如 { X } 的式子，用于提取属性文法
    smatch sm;
    vector<Production> productions;
    ifstream prod_file(fileName);
    string line;
    while (getline(prod_file, line)) {
        // 忽略注释
        if (line[0] == '/' && line[1] == '/') {
            continue;
        }
        if (regex_search(line, sm, e1)) {
            string lhs = sm[1];
            vector<string> rhs;
            line = sm[2];
            while (regex_search(line, sm, e2)) {
                rhs.push_back(sm[1]);
                line = sm[2];
            }
            rhs.emplace_back(line);  // 最后一个符号
            Production p(lhs, rhs);
            productions.push_back(p);
        } else if (regex_search(line, sm, e3)) {
            string attr = sm[1];
            productions.back().attr.emplace_back(attr);
        }
    }
    prod_file.close();
    return productions;
}
// 将多个左部相同的产生式合并为一个产生式
vector<MultiProduction> getMultiProductions(vector<Production>& productions) {
    vector<MultiProduction> multiProductions;
    map<string, vector<vector<string>>> mp;
    for (const auto& i : productions) {
        mp[i.lhs].push_back(i.rhs);
    }
    for (const auto& i : mp) {
        MultiProduction p(i.first, i.second);
        multiProductions.push_back(p);
    }
    return multiProductions;
}