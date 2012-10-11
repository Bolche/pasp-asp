#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <limits>
#include <Eigen/LU>
#include <cmath>

#include "Program.h"
#include "Bridge.h"

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

void Program::addRule(Rule_ptr r) {
    rules.push_back(r);
}

list<Rule_ptr>::const_iterator Program::rulesBeginIterator() const {
    return rules.begin();
}
list<Rule_ptr>::const_iterator Program::rulesEndIterator() const {
    return rules.end();
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

    Eigen::MatrixXd inverseBase = base.inverse();
    Eigen::VectorXd pi = inverseBase * p;
    while (costs.dot(pi) > 0) {
        Eigen::VectorXd A = selectColumn(inverseBase, base.determinant(), costs);
        if (A.size() == 0)
            throw false;
        changeBase(base, inverseBase, A, pi, costs);
        pi = inverseBase * p;
    }

    return make_pair(base, pi);
}

Eigen::VectorXd Program::selectColumn(const Eigen::MatrixXd &inverseBase, const double determinant, const Eigen::VectorXd &costs) const {
    Eigen::RowVectorXd u = (costs.transpose() * inverseBase) * determinant;
    unordered_map<Literal, long> weightConstraint;
    unsigned int i;
    map<Literal, double>::const_iterator it;
    for(it = probabilityTable.begin(), i=1; it != probabilityTable.end(); it++, i++) {
        weightConstraint.insert(make_pair(it->first, u[i]));
    }

    Program newProg(*this);
    newProg.addRule(make_shared<WeightRule>(weightConstraint, -u[0]));
    try {
        return answerSetToVector(bridge->getAnswerSet(newProg));
    } catch (bool e) {
        Eigen::VectorXd v;
        return v;
    }
}

Eigen::VectorXd Program::answerSetToVector(const unordered_set<Literal>& as) const {
    Eigen::VectorXd v(numProbabilities()+1);
    unsigned int i;
    map<Literal, double>::const_iterator it;
    for(it = probabilityTable.begin(), i=1; it != probabilityTable.end(); it++, i++) {
        v[i] = as.count(it->first);
    }
    v[0] = 1;
    return v;
}

void Program::changeBase(Eigen::MatrixXd &base, Eigen::MatrixXd &inverseBase, const Eigen::VectorXd &A, const Eigen::VectorXd &pi, Eigen::VectorXd &costs) const {
    double minTheta = numeric_limits<double>::infinity();
    unsigned int minIndex = 0;
    Eigen::VectorXd u = inverseBase * A;
    Eigen::VectorXd theta = pi.cwiseQuotient(u);
    for(unsigned int i=0; i < u.size(); i++) {
        if (u[i] > 0) {
            if(theta[i] < minTheta) {
                minTheta = theta[i];
                minIndex = i;
            }
        }
    }

    base.col(minIndex) = A;
    inverseBase = base.inverse();
    costs[minIndex] = 0;
}

void Program::setInitialBase(Eigen::MatrixXd &m) const {
    for(unsigned int i = 0; i <= numProbabilities(); i++)
        for(unsigned int j = 0; j <= numProbabilities(); j++)
            m(i,j) = (i<=j);
}

bool Program::consistent(const Eigen::VectorXd &v) const {
    int i;
    map<Literal, double>::const_iterator it;
    Program newProg(*this);
    for(it = probabilityTable.begin(), i=1; it != probabilityTable.end(); it++, i++) {
        if (!v[i])
            newProg.addRule(make_shared<ConstraintRule>(vector<Literal>({it->first}), vector<Literal>()));
        else
            newProg.addRule(make_shared<ConstraintRule>(vector<Literal>(), vector<Literal>({it->first})));
    }

    return newProg.consistent();        
}

bool Program::consistent() const {
    return bridge->existsAnswerSet(*this);
}
