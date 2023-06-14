#include "Semer.h"

#include <iostream>
#include <regex>
using namespace std;

const regex r_assign("(.+) = (.+)");        // 形如 A = B
const regex r_func("([^\\s]+)[(](.*)[)]");  // 形如 func()
const regex r_attr("(.+)[.](.+)");          // 形如 A.attr

int Semer::t_num = 0;
int Semer::l_num = 0;
int Semer::i_num = 0;

/*
 *	@brief	字符串分割函数
 */
vector<string> split(string str, string pattern) {
    string::size_type pos;
    vector<string> result;
    str += pattern;  // 扩展字符串以方便操作
    size_t size = str.size();
    for (size_t i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

Semer::Semer() {
    ast = new AST();
}

AST* Semer::get_ast() {
    return this->ast;
}

/*
 *	@brief 执行一条产生式附带的语义规则
 */
AST_NODE* Semer::execute(vector<string> pd, vector<string> rules, stack<AST_NODE*>& nt) {
    smatch sm;
    AST_NODE* node = new AST_NODE();

    // 建立由父到子节点的关联
    node->val = pd[0] + "0";
    for (size_t i = pd.size() - 1; i > 0; i--) {
        if (pd[i] != "#") {
            string name = pd[i] + to_string(i);
            AST_NODE* pnode = nt.top();
            node->children.insert({name, pnode});
            nt.pop();
        }
    }

    for (auto rule : rules) {
        // 属性操作
        if (regex_match(rule, sm, r_assign)) {
            string left = sm.str(1);   // 左值
            string right = sm.str(2);  // 右值

            if (regex_match(left, sm, r_attr)) {
                string attr = sm.str(2);
                if (attr == "addr") {
                    set_addr(node, right);
                }

                if (attr == "code") {
                    set_code(node, right);
                }

                if (attr == "truelb") {
                    set_true(node, right);
                }

                if (attr == "falselb") {
                    set_fals(node, right);
                }
                if (attr == "nextlb") {
                    set_next(node, right);
                }
            }
        }

        // 函数调用
        if (regex_match(rule, sm, r_func)) {
            string func_name = sm.str(1);  // 函数名
            string argstr = sm.str(2);     // 参数列表

            // 新建符号表
            if (func_name == "stab") {
                map<string, list<SYM*>>* sym_tab = new map<string, list<SYM*>>();
                this->sym_st.push(sym_tab);
            }

            // 创建符号表条目
            if (func_name == "sym") {
                auto* top_tab = sym_st.top();
                vector<string> args = split(argstr, ",");
                for (size_t i = 0; i < args.size(); i++) {
                    args[i] = get_attr(node, args[i]);
                }
                SYM* sym = new SYM{args[1]};
                for (const auto& i : *top_tab) {
                    for (const auto& j : i.second) {
                        if (j->type == sym->type) {
                            cout << "Redefinition Error at : " << args[1] << endl;
                        }
                    }
                }
                auto it = top_tab->find(args[0]);
                if (it == top_tab->end()) {
                    top_tab->insert({args[0], {sym}});
                } else {
                    it->second.push_back(sym);
                }
            }

            // 删除符号表
            if (func_name == "delstab") {
                auto* ptr = sym_st.top();
                delete ptr;
                this->sym_st.pop();
            }

            // 回填
            if (func_name == "backpatch") {
                vector<string> args = split(argstr, ",");
                for (size_t i = 0; i < args.size(); i++) {
                    args[i] = get_attr(node, args[i]);
                }
                label_map.insert({args[0], args[1]});
            }
        }
    }

    ast->set_root(node);
    return node;
}

/*
 *	@brief 为节点的addr属性赋值
 */
void Semer::set_addr(AST_NODE* node, string right) {
    smatch sm;

    // 右值为函数
    if (regex_search(right, sm, r_func)) {
        string func = sm.str(1);

        if (func == "temp") {
            node->addr = "t" + to_string(t_num++);
        }

        if (func == "label") {
            node->addr = "L" + to_string(l_num++);
        }
    }

    // 右值为属性
    else if (regex_search(right, sm, r_attr)) {
        node->addr = get_attr(node, sm.str(0));
    }
}

/*
 *	@brief 为节点的truelb属性赋值
 */
void Semer::set_true(AST_NODE* node, string right) {
    smatch sm;

    // 右值为函数
    if (regex_match(right, sm, r_func)) {
        string func = sm.str(1);
        string argstr = sm.str(2);

        if (func == "instr") {
            node->truelb = "i" + to_string(i_num++);
        }

        if (func == "merge") {
            vector<string> args = split(argstr, ",");
            string a1 = get_attr(node, args[0]);
            string a2 = get_attr(node, args[1]);

            node->truelb = "i" + to_string(i_num++);
            label_map.insert({a1, node->truelb});
            label_map.insert({a2, node->truelb});
        }
    }

    // 右值为属性
    else if (regex_match(right, sm, r_attr)) {
        node->truelb = get_attr(node, sm.str(0));
    }
}

/*
 *	@brief 为节点的falselb属性赋值
 */
void Semer::set_fals(AST_NODE* node, string right) {
    smatch sm;

    // 右值为函数
    if (regex_match(right, sm, r_func)) {
        string func = sm.str(1);
        string argstr = sm.str(2);

        if (func == "instr") {
            node->falselb = "i" + to_string(i_num++);
        }

        if (func == "merge") {
            vector<string> args = split(argstr, ",");
            string a1 = get_attr(node, args[0]);
            string a2 = get_attr(node, args[1]);

            node->falselb = "i" + to_string(i_num++);
            label_map.insert({a1, node->falselb});
            label_map.insert({a2, node->falselb});
        }
    }

    // 右值为属性
    else if (regex_match(right, sm, r_attr)) {
        node->falselb = get_attr(node, sm.str(0));
    }
}

/*
 *	@brief 为节点的nextlb属性赋值
 */
void Semer::set_next(AST_NODE* node, string right) {
    smatch sm;

    // 右值为函数
    if (regex_match(right, sm, r_func)) {
        string func = sm.str(1);
        string argstr = sm.str(2);

        if (func == "instr") {
            node->nextlb = "i" + to_string(i_num++);
        }

        if (func == "merge") {
            vector<string> args = split(argstr, ",");
            node->nextlb = "i" + to_string(i_num++);
            for (auto arg : args) {
                arg = get_attr(node, arg);
                label_map.insert({arg, node->nextlb});
            }
        }
    }

    // 右值为属性
    else if (regex_match(right, sm, r_attr)) {
        node->nextlb = get_attr(node, sm.str(0));
    }
}

/*
 *	@brief 为指令的code属性赋值
 */
void Semer::set_code(AST_NODE* node, string right) {
    smatch sm;

    vector<string> parts = split(right, "||");
    for (auto part : parts) {
        // 子节点的code属性
        if (regex_search(part, sm, r_attr) && sm.str(2) == "code") {
            vector<QUAD*> tmp = get_code(node, sm.str(1));
            if (!tmp.empty()) {
                for (auto each : tmp) {
                    node->code.push_back(each);
                }
            }
        }

        // 新建QUAD
        if (regex_search(part, sm, r_func) && sm.str(1) == "quad") {
            string argstr = sm.str(2);
            vector<string> args = split(argstr, ",");
            for (size_t i = 0; i < args.size(); i++) {
                args[i] = get_attr(node, args[i]);
            }

            QUAD* quad = new QUAD{args[0], args[1], args[2], args[3]};
            node->code.push_back(quad);

            // quad->print();
        }
    }
}

/*
 *	@brief 获取字符串代表的属性值
 */
string Semer::get_attr(AST_NODE* node, string attr) {
    smatch sm;
    AST_NODE* sub;
    if (regex_search(attr, sm, r_attr)) {
        string node_name = sm.str(1);  // 节点名称
        string attr_name = sm.str(2);  // 节点属性
        if (node_name == node->val) {
            sub = node;
        } else {
            sub = node->children[node_name];
        }

        if (attr_name == "val") {
            return sub->val;
        }
        if (attr_name == "addr") {
            return sub->addr;
        }
        if (attr_name == "truelb") {
            return sub->truelb;
        }
        if (attr_name == "falselb") {
            return sub->falselb;
        }
        if (attr_name == "nextlb") {
            return sub->nextlb;
        }
    }
    return attr;
}

/*
 *	@brief 获取指定子节点的code属性
 */
vector<QUAD*> Semer::get_code(AST_NODE* node, string node_name) {
    AST_NODE* sub = node->children[node_name];
    return sub->code;
}

/*
 *	@brief 获取真标号
 */
string Semer::get_true_label(string instr) {
    while (!instr.empty()) {
        if (instr[0] == 'L')
            return instr;
        instr = label_map[instr];
    }
    return "-";
}

void Semer::print() {
    cout << endl
         << "[LABEL MAP]" << endl;
    for (auto each : label_map) {
        cout << each.first << " -> " << each.second << endl;
    }
}