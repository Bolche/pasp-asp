/* 
 * File:   Program.cpp
 * Author: eduardo
 * 
 * Created on 7 de Junho de 2011, 02:59
 */

#include <algorithm>
#include <sstream>
#include "Program.h"

Rule::Rule(const Literal h, const vector<Literal>& p, const vector<Literal>& n): head(h), positiveBody(p), negativeBody(n) {}

Rule_ptr Program::newRule(const Literal h, const vector<Literal>& p, const vector<Literal>& n) {
    Rule_ptr newRule = make_shared<Rule> (h, p, n);
    rules.insert(make_pair(h, newRule));
    
    insertLiteralsInTable(h, p, n);
    
    return newRule;
}

void Program::insertLiteralsInTable(const Literal h, const vector<Literal>& p, const vector<Literal>& n) {
    string emptyName;
    symTable[h] = emptyName;
    setBiggestLiteral(h);
    for (auto lit : p) {
        symTable[lit] = emptyName;
        setBiggestLiteral(lit);
    }
    for (auto lit : n) {
        symTable[lit] = emptyName;
        setBiggestLiteral(lit);
    }
}

inline void Program::setBiggestLiteral(const Literal& l) {
    if (l > biggestLiteral)
        biggestLiteral = l;
}
    
unsigned int Program::numLiterals() const {
    return symTable.size();
}

unsigned int Program::numProbabilities() const {
    return probabilityTable.size();
}

unsigned int Program::numDecimals() const {
    unsigned int maxDecimals = 0;
    for (auto begin = probabilityTable.begin(); begin != probabilityTable.end(); begin++)
        if (begin->second.size() > maxDecimals)
            maxDecimals = begin->second.size();
    if (maxDecimals <= 2) return maxDecimals;
    return maxDecimals - 2;
}

Literal Program::lastLiteral() const {
    return biggestLiteral;
}

void Program::nameLiteral(const Literal& l, const string& n) {
    symTable[l]=n;
}

void Program::assignProbability(const string& literalName, const string& n) {
    auto begin = symTable.begin();
    for (; begin!=symTable.end(); begin++)
        if ( begin->second == literalName )
            break;

    if (begin != symTable.end())
        probabilityTable[begin->first]=n;
}

//TODO isso não deveria estar aqui. Isso é específico do DIMACS CNF
string Program::getProbabilityAssignment() const {
    stringstream result;
    for (auto begin = probabilityTable.begin(); begin != probabilityTable.end(); begin++)
        result << "a " << begin->first << ' ' << begin->second << endl;
    
    return result.str();
}

string Program::getLiteralName(const Literal& l) const {
    return symTable.at(l);
}

string Program::getLiteralProbability(const Literal& l) const {
    return probabilityTable.at(l);
}

ruleRange Program::getRulesWithHead(const Literal head) const {
    return rules.equal_range(head);
}

void Program::reassignLiteralNumbers() {
    map<Literal, Literal> translationTable;
    Literal nextAvaliableLiteral(1);
    //Create a translation table
    for (auto entry: probabilityTable)
        translationTable[entry.first] = nextAvaliableLiteral++;

    for (Literal lit = Literal(1); lit <= lastLiteral(); lit++)
        if (translationTable.count(lit) == 0)
            translationTable[lit] = nextAvaliableLiteral++;

    //do the reassignment
    multimap<Literal, Rule_ptr> newRules;
    unordered_map<Literal, string> newSymTable;
    map<Literal, string> newProbabilityTable;
    for (auto entry: probabilityTable)
        newProbabilityTable[translationTable[entry.first]] = entry.second;
    probabilityTable.swap(newProbabilityTable);

    for (auto entry: symTable)
        newSymTable[translationTable[entry.first]] = entry.second;
    symTable.swap(newSymTable);
    
    for(auto rule: rules) {
        newRules.insert(make_pair(translationTable[rule.first], rule.second));
        rule.second->head = translationTable[rule.second->head];
        for(Literal &lit: rule.second->positiveBody)
            lit = translationTable[lit];
        for(Literal &lit: rule.second->negativeBody)
            lit = translationTable[lit];
    }
    rules.swap(newRules);
}
