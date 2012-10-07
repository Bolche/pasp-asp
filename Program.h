#ifndef PROGRAM_H
#define	PROGRAM_H

#include <string>
#include <map>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <Eigen/Core>

#include "c++11_warning.h"
#include "Rule.h"

class Bridge;

using namespace std;

/* Represents a program */
class Program {
public:
    Program (Bridge *b) : bridge(b) {};
    virtual ~Program() {};
    void addRule(Rule_ptr);
    void nameLiteral(const Literal& l, const string& n);
    void assignProbability(const string& literalName, double n);
    string getLiteralName(const Literal& l) const;
    double getLiteralProbability(const Literal& l) const;
    unsigned int numLiterals() const;
    unsigned int numProbabilities() const;
    void initializeBase(Eigen::MatrixXd& m) const;
    bool consistent() const;
    bool consistent(const Eigen::VectorXd &v) const;
    pair<Eigen::MatrixXd, Eigen::VectorXd> solve() const;
    list<Literal> positiveTargets;
    list<Literal> negativeTargets;
private:
    void setInitialBase(Eigen::MatrixXd &m) const;
    Eigen::VectorXd selectColumn(const Eigen::MatrixXd &base, const Eigen::VectorXd &costs) const;
    void changeBase(Eigen::MatrixXd&, Eigen::MatrixXd&, const Eigen::VectorXd&, const Eigen::VectorXd&, Eigen::VectorXd&) const;
    Eigen::VectorXd answerSetToVector(const unordered_set<Literal>&) const;
    list<Rule_ptr> rules;
    unordered_map<Literal, string> symTable;
    map<Literal, double> probabilityTable;
    Bridge *bridge;
};

#endif	/* PROGRAM_H */
