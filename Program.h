#ifndef PROGRAM_H
#define	PROGRAM_H

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <Eigen/Core>

#include "c++11_warning.h"
#include "Bridge.h"

using namespace std;
/* Represents a Literal */
typedef int Literal;

namespace Eigen {
typedef Eigen::Matrix<bool, Dynamic, Dynamic> MatrixXb;
typedef Eigen::Vector<bool, Dynamic, Dynamic> VectorXb;
}

/* Represents a program */
class Program {
public:
    Program () {};
    virtual ~Program() {};
    void newRule(const string);
    void nameLiteral(const Literal& l, const string& n);
    void assignProbability(const string& literalName, const string& n);
    string getLiteralName(const Literal& l) const;
    string getLiteralProbability(const Literal& l) const;
    unsigned int numLiterals() const;
    unsigned int numProbabilities() const;
    unsigned int numDecimals() const;
    void initializeBase(Eigen::MatrixXb& m) const;
    bool consistent() const;
    bool consistent(const Eigen::VectorXb &v) const;
    void solve();
    unsigned int numModels;
private:
    list<string> rules;
    unordered_map<Literal, string> symTable;
    map<Literal, string> probabilityTable;
    Bridge *bridge;
};

#endif	/* PROGRAM_H */
