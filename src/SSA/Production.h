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

private:
};

class MultiProduction {
public:
    MultiProduction(string lhs, vector<vector<string>> rhs);
    string lhs;
    vector<vector<string>> rhs;
    friend ostream& operator<<(ostream& os, const MultiProduction& p);
};

vector<Production> getProductions(string filename);

vector<MultiProduction> getMultiProductions(vector<Production>& productions);