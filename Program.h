/* 
 * File:   Program.h
 * Author: eduardo
 *
 * Created on 7 de Junho de 2011, 02:59
 */

#ifndef PROGRAM_H
#define	PROGRAM_H

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>

#include "c++11_warning.h"

using namespace std;
/* Represents a Literal */
typedef int Literal;

/* Represents a rule */
struct Rule {
    Rule(const Literal, const vector<Literal>& , const vector<Literal>&);
    Literal head;
    vector<Literal> positiveBody;
    vector<Literal> negativeBody;
};

typedef shared_ptr<Rule> Rule_ptr;

typedef pair<multimap<Literal,Rule_ptr>::const_iterator, multimap<Literal,Rule_ptr>::const_iterator> ruleRange;

/* Represents a program */
class Program {
public:
    Program (): biggestLiteral(0) {};
    virtual ~Program() {};
    Rule_ptr newRule(const Literal, const vector<Literal>&, const vector<Literal>&);
    void nameLiteral(const Literal& l, const string& n);
    void assignProbability(const string& literalName, const string& n);
    string getLiteralName(const Literal& l) const;
    string getLiteralProbability(const Literal& l) const;
    unsigned int numLiterals() const;
    unsigned int numProbabilities() const;
    unsigned int numDecimals() const;
    string getProbabilityAssignment() const;
    Literal lastLiteral() const;
    unsigned int numModels;
    ruleRange getRulesWithHead(const Literal head) const;
    void reassignLiteralNumbers();
private:
    void insertLiteralsInTable(const Literal, const vector<Literal>&, const vector<Literal>&);
    inline void setBiggestLiteral(const Literal&);
    Literal biggestLiteral;
    multimap<Literal, Rule_ptr> rules;
    unordered_map<Literal, string> symTable;
    map<Literal, string> probabilityTable;
};

#endif	/* PROGRAM_H */

