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

bool Item::operator==(const Item& item) const {
    return prod_id == item.prod_id && dot_pos == item.dot_pos &&
           lookahead == item.lookahead;
}

Praser::Praser(const vector<Token>& tokens, string filename, Semer& semer) {
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
    build_collection();
    build_action();
    build_goto();

    // cout << "First集: " << endl;
    // print_first();
    // cout << endl;

    // cout << "Trans" << endl;
    // print_trans();

    // cout << "Action\n";
    // print_Action();

    stack<int> st;        // 状态栈
    stack<AST_NODE*> nt;  // 语法树节点栈
    st.push(0);
    for (size_t i = 0; i < tokens.size();) {
        auto* token = &tokens[i];
        string inputs;
        if (token->type == "ID")
            inputs = "ID";
        else if (token->type == "INT" || token->type == "FLOAT" || token->type == "E_NUM" || token->type == "I_NUMF")
            inputs = "NUM";
        else
            inputs = token->content;

        int op = ACTION(st.top(), inputs);

        if (STATE_STACK_OUT) {
            cout << "INPUT: " << inputs << "\t";
            cout << "STACK TOP: " << st.top() << "\t";
        }

        // 报错
        if (op == ERROR) {
            res = 0;
            cout << "Syntax error at line: " << token->line << endl;
            cout << "Incorrect syntax: " << token->content << endl
                 << endl;
            break;
        }

        // 接受
        if (op == ACC) {
            res = 1;
            if (STATE_STACK_OUT)
                cout << "accept" << endl;
            break;
        }

        // 移入
        if (op <= 0) {
            AST_NODE* leaf = new AST_NODE();
            leaf->val = token->content;
            nt.push(leaf);

            st.push(-op);
            ++i;

            if (STATE_STACK_OUT)
                cout << "shift " << -op << endl;
        }

        // 规约
        if (op > 0) {
            // vector<string> pd = sdd.get_pd(op);
            Production pd = productions[op];

            // 将产生式体数量的状态弹出栈
            for (size_t j = 1; j < pd.rhs.size(); j++) {
                if (st.size() > 1 && pd.rhs[j] != "#")
                    st.pop();
            }
            // 新状态压栈
            st.push(GOTO(st.top(), pd.lhs));

            AST_NODE* node = semer.execute(pd, pd.attr, nt);
            nt.push(node);

            if (STATE_STACK_OUT)
                cout << "reduce " << op << " -> " << pd.lhs << endl;
        }
    }
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
    // build_first();
    for (const auto& i : first) {
        cout << i.first << ": ";
        for (const auto& j : i.second) {
            cout << j << " ";
        }
        cout << endl;
    }
}

void Praser::print_trans() {
    for (const auto& i : trans) {
        cout << i.first.first << " " << i.first.second << " " << i.second
             << endl;
    }
}

void Praser::print_Action() {
    for (const auto& i : action) {
        cout << i.first.first << " " << i.first.second << " " << i.second << endl;
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

set<Item> Praser::get_closure(const set<Item>& items) {
    set<Item> ret;
    for (const auto& item : items) {
        auto t = get_closure(item);
        ret.insert(t.begin(), t.end());
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
    return get_closure(ret);
}

void Praser::build_collection() {
    item_collection.emplace_back(get_closure(Item{0, 0, {"$"}}));

    bool flag = true;
    while (flag) {
        flag = false;
        for (int i = 0; i < item_collection.size(); i++) {
            set<string> symbols(
                terminals.begin(), terminals.end());  // 终结符
            symbols.insert(nonterminals.begin(),
                           nonterminals.end());  // 非终结符
            symbols.erase("#");
            for (const auto& sym : symbols) {
                set<Item> tmp = get_goto(item_collection[i], sym);

                if (!tmp.empty()) {
                    auto it = find(item_collection.begin(), item_collection.end(), tmp);

                    if (it == item_collection.end()) {
                        item_collection.emplace_back(tmp);
                        flag = true;
                        trans.insert({{i, sym}, item_collection.size() - 1});
                    } else {
                        trans.insert({{i, sym}, it - item_collection.begin()});
                    }
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
                // for (const auto& lookahead : item.lookahead) {
                //     if (p.lhs != START_SYM)
                //         action.insert({{item.prod_id, lookahead}, item.prod_id});
                //     else
                //         action.insert({{item.prod_id, lookahead}, ACC});
                // }
                if (p.lhs != START_SYM) {
                    for (const auto& lookahead : item.lookahead) {
                        auto it = action.find({i, lookahead});
                        if (it != action.end()) {
                            if (it->second != item.prod_id) {
                                cout << "conflict: " << i << " " << lookahead
                                     << " " << it->second << " " << item.prod_id
                                     << endl;
                            }
                        } else {
                            action[{i, lookahead}] = item.prod_id;
                        }
                    }
                } else {
                    action[{i, "$"}] = ACC;
                }
            } else if (nonterminals.find(p.rhs[item.dot_pos]) == nonterminals.end()) {  // 移入
                string s = p.rhs[item.dot_pos];
                pair<int, string> pair(i, s);
                if (trans.find(pair) != trans.end()) {
                    auto it = action.find(pair);
                    if (it != action.end()) {
                        cout << "-----------------------------------------------" << endl;
                    }
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

int Praser::GOTO(int state, string symbol) {
    pair<int, string> p(state, symbol);
    if (goto_.find(p) != goto_.end()) {
        return goto_[p];
    } else {
        return ERROR;
    }
}

int Praser::ACTION(int state, string symbol) {
    pair<int, string> p(state, symbol);
    if (action.find(p) != action.end()) {
        return action[p];
    } else {
        return ERROR;
    }
}