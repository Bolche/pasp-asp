#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <Eigen/Cholesky>
#include <Eigen/LU>

#include "Program.h"
#include "Bridge.h"

unsigned int Program::numLiterals() const {
    return symTable.size();
}

unsigned int Program::numProbabilities() const {
    return probabilityTable.size();
}

void Program::nameLiteral(const Literal& l, const string& n) {
    symTable[l]=n;
}

void Program::assignProbability(const string& literalName, double n) {
    auto begin = symTable.begin();
    for (; begin!=symTable.end(); begin++)
        if ( begin->second == literalName )
            break;

    if (begin != symTable.end())
        probabilityTable[begin->first]=n;
}

string Program::getLiteralName(const Literal& l) const {
    return symTable.at(l);
}

double Program::getLiteralProbability(const Literal& l) const {
    return probabilityTable.at(l);
}

void Program::addWeightConstraint(const unordered_map<Literal, double>& weights, double limit) {

}

void Program::addConstraint(const vector<Literal>& positiveLiterals, const vector<Literal>& negativeLiterals) {

}

pair<Eigen::MatrixXd, Eigen::VectorXd> Program::solve() const {
    unsigned int matrixSize = numProbabilities()+1;
    Eigen::MatrixXd base(matrixSize, matrixSize);
    setInitialBase(base);
    Eigen::VectorXd costs = Eigen::VectorXd::Ones(matrixSize);
    for (unsigned int i=0; i < matrixSize; i++)
        if (consistent(base.col(i)))
            costs(i) = 0;

    Eigen::VectorXd p(matrixSize);
    unsigned int i;
    map<Literal, double>::const_iterator it;
    for(it = probabilityTable.begin(), i=1; it != probabilityTable.end(); it++, i++)
        p[i] = it->second;
    p[0] = 1;

    Eigen::VectorXd pi = base.ldlt().solve(p);
    while (costs.dot(pi) > 0) {
        Eigen::VectorXd A = selectColumn(base, costs);
        if (A.size() == 0)
            throw false;
        changeBase(base, A, pi, costs);
        pi = base.ldlt().solve(p);
    }

    return make_pair(base, pi);
}

Eigen::VectorXd Program::selectColumn(const Eigen::MatrixXd &base, const Eigen::VectorXd &costs) const {
    Eigen::RowVectorXd u = costs.transpose() * base.inverse();
    unordered_map<Literal, double> weightConstraint;
    unsigned int i;
    map<Literal, double>::const_iterator it;
    for(it = probabilityTable.begin(), i=1; it != probabilityTable.end(); it++, i++) {
        weightConstraint.insert(make_pair(it->first, u[i]));
    }

    Program newProg(*this);
    newProg.addWeightConstraint(weightConstraint, -u[0]);
    return answerSetToVector(bridge->getAnswerSet(newProg));
}

Eigen::VectorXd Program::answerSetToVector(const unordered_set<Literal>& as) const {
    Eigen::VectorXd v(numProbabilities()+1);
    unsigned int i;
    map<Literal, double>::const_iterator it;
    for(it = probabilityTable.begin(), i=1; it != probabilityTable.end(); it++, i++)
        v[i] = as.count(it->first);
    v[0] = 1;
    return v;
}

void Program::changeBase(Eigen::MatrixXd &base, const Eigen::VectorXd &A, const Eigen::VectorXd &pi, Eigen::VectorXd &costs) const {

}

void Program::setInitialBase(Eigen::MatrixXd &m) const {
    for(unsigned int i = 0; i <= numProbabilities(); i++)
        for(unsigned int j = 0; j <= numProbabilities(); j++)
            m(i,j) = (i<=j);
}

bool Program::consistent(const Eigen::VectorXd &v) const {
    int i;
    map<Literal, double>::const_iterator it;
    vector<Literal> positiveLiterals, negativeLiterals;
    for(it = probabilityTable.begin(), i=1; it != probabilityTable.end(); it++, i++) {
        if (!v[i])
            positiveLiterals.push_back(it->first);
        else
            negativeLiterals.push_back(it->first);
    }

    Program newProg(*this);
    newProg.addConstraint(positiveLiterals, negativeLiterals);
    return newProg.consistent();        
}

bool Program::consistent() const {
    return true;
}
