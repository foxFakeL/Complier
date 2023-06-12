#include "Praser.h"

bool Item::operator<(const Item& item) const {
    if (prod_id < item.prod_id) {
        return true;
    } else if (prod_id == item.prod_id) {
        if (dot_pos < item.dot_pos) {
            return true;
        } else if (dot_pos == item.dot_pos) {
            if (lookahead < item.lookahead) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

Praser::Praser(const vector<Token>& tokens, string filename) {
    this->tokens = tokens;
    this->productions = getProductions(filename);
    for (auto i : productions) {
        nonterminals.insert(i.lhs);
        for (auto j : i.rhs) {
            if (j[0] == '\'' || j[0] == '\"') {
                continue;
            }
            terminals.insert(j);
        }
    }
    for (auto i : nonterminals) {
        terminals.erase(i);
    }

    build_first();
}

// 获取一个串的First集
set<string> Praser::get_first(const vector<string>& vec) {
    set<string> ret;
    for (const auto& i : vec) {
        auto t = first[i];
        ret.insert(t.begin(), t.end());
        if (t.find("#") == t.end()) {
            ret.erase("#");
            break;
        }
    }
    return ret;
}

// 生成 first 集
void Praser::build_first() {
    for (const auto& t : terminals) {
        first[t].insert(t);
    }
    first["#"].insert("#");
    bool flag = true;
    while (flag) {
        flag = false;
        for (const auto& prod : productions) {
            auto tmp = get_first(prod.rhs);
            tmp.insert(first[prod.lhs].begin(), first[prod.lhs].end());

            if (tmp.size() != first[prod.lhs].size()) {
                flag = true;
                first[prod.lhs] = tmp;
            }
        }
    }
}

void Praser::print_first() {
    build_first();
    for (const auto& i : first) {
        cout << i.first << ": ";
        for (const auto& j : i.second) {
            cout << j << " ";
        }
        cout << endl;
    }
}

set<Item> Praser::get_closure(const Item& item) {
    set<Item> ret;
    ret.insert(item);
    bool flag = true;
    while (flag) {
        flag = false;
        for (auto i : ret) {
            if (i.dot_pos == productions[i.prod_id].rhs.size()) {
                continue;
            }
            string next = productions[i.prod_id].rhs[i.dot_pos];
            if (nonterminals.find(next) == nonterminals.end()) {
                continue;
            }
            for (int k = 0; k < productions.size(); k++) {
                auto& j = productions[k];
                if (j.lhs == next) {
                    Item tmp;
                    tmp.prod_id = k;
                    tmp.dot_pos = 0;
                    tmp.lookahead = get_first(
                        vector<string>(j.rhs.begin() + 1, j.rhs.end()));
                    if (tmp.lookahead.find("#") != tmp.lookahead.end()) {
                        tmp.lookahead.erase("#");
                        tmp.lookahead.insert(item.lookahead.begin(),
                                             item.lookahead.end());
                    }
                    if (ret.find(tmp) == ret.end()) {
                        ret.insert(tmp);
                        flag = true;
                    }
                }
            }
        }
    }
    return ret;
}

set<Item> Praser::get_goto(const set<Item>& items, const string& symbol) {
    set<Item> ret;
    for (auto i : items) {
        if (i.dot_pos == productions[i.prod_id].rhs.size()) {
            continue;
        }
        if (productions[i.prod_id].rhs[i.dot_pos] == symbol) {
            Item tmp;
            tmp.prod_id = i.prod_id;
            tmp.dot_pos = i.dot_pos + 1;
            tmp.lookahead = i.lookahead;
            ret.insert(tmp);
        }
    }
    set<Item> true_ret;
    for (const auto& i : ret) {
        auto t = get_closure(i);
        true_ret.insert(t.begin(), t.end());
    }
    return true_ret;
}

void Praser::build_collection() {
    item_collection.emplace_back(get_closure(Item{0, 0, {"$"}}));

    bool flag = true;
    while (flag) {
        flag = false;
        for (int i = 0; i < item_collection.size(); i++) {
            for (auto j : terminals) {
                auto t = get_goto(item_collection[i], j);
                if (t.size() == 0) {
                    continue;
                }
                flag = true;
                auto it = find(item_collection.begin(), item_collection.end(), t);
                if (it == item_collection.end()) {
                    trans.insert({{i, j}, item_collection.size()});
                    item_collection.emplace_back(t);
                } else {
                    trans.emplace(make_pair(i, j), it - item_collection.begin());
                }
            }
            for (auto j : nonterminals) {
                auto t = get_goto(item_collection[i], j);
                if (t.size() == 0) {
                    continue;
                }
                flag = true;
                auto it = find(item_collection.begin(), item_collection.end(), t);
                if (it == item_collection.end()) {
                    trans.insert({{i, j}, item_collection.size()});
                    item_collection.emplace_back(t);
                } else {
                    trans.emplace(make_pair(i, j), it - item_collection.begin());
                }
            }
        }
    }
}

void Praser::build_action() {
    int i = 0;
    for (const auto& items : item_collection) {
        for (const auto& item : items) {
            Production p = productions[item.prod_id];

            // 点在产生式最末 或 点后为空（规约或接受）
            if (p.rhs.size() == item.dot_pos || p.rhs[item.dot_pos] == "#") {
                for (const auto& lookahead : item.lookahead) {
                    if (p.lhs != START_SYM)
                        action.insert({{item.prod_id, lookahead}, item.prod_id});
                    else
                        action.insert({{item.prod_id, lookahead}, ACC});
                }
            } else if (nonterminals.find(p.rhs[item.dot_pos]) == nonterminals.end()) {  // 移入
                string s = p.rhs[item.dot_pos];
                pair<int, string> p(i, s);
                if (trans.find(p) != trans.end()) {
                    action[p] = -trans[p];
                }
            }
        }
        i++;
    }
}

void Praser::build_goto() {
    for (auto i : trans) {
        string sym = i.first.second;
        if (nonterminals.find(sym) != nonterminals.end()) {
            goto_[make_pair(i.first.first, sym)] = i.second;
        }
    }
}