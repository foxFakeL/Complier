#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>
using namespace std;

class Production {
public:
    Production(string lhs, vector<string> rhs);
    string lhs;
    vector<string> rhs;
    vector<string> attr;
    friend ostream& operator<<(ostream& os, const Production& p);

    string& operator[](int) const noexcept;

private:
};
