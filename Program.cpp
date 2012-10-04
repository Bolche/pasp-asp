#include <algorithm>
#include <sstream>
#include <cstdlib>

#include <Eigen/Cholesky>
#include "Program.h"


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

string Program::getLiteralName(const Literal& l) const {
    return symTable.at(l);
}

string Program::getLiteralProbability(const Literal& l) const {
    return probabilityTable.at(l);
}

void Program::solve() {
    unsigned int matrixSize = numProbabilities()+1;
    Eigen::MatrixXb base(matrixSize, matrixSize);
    Eigen::VectorXd costs = Eigen::VectorXd::Ones(matrixSize);
    for (int i=0; i < matrixSize; i++)
        if (consistent(base(i)))
            costs(i) = 0;

    Eigen::VectorXd p;
    int i;
    probabilityTable::const_iterator it;
    for(it = probabilityTable.begin(), i=0; it != probabilityTable.end(); it++, i++)
        p[i] = atof(it->second.c_str());

    Eigen::VectorXd pi = base.ldlt().solve(p);

    while (costs.dot(pi) > 0) {
        
    }
    
    setInitialBase(base);
}

void Program::setInitialBase(Eigen::MatrixXb &m) const {
    for(int i = 0; i <= numProbabilities(); i++)
        for(int i = 0; i <= numProbabilities(); i++)
            m(i,j) = (i<=j);
}

bool Program::consistent(const Eigen::VectorXb &v) const {
    int i;
    probabilityTable::const_iterator it;
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

}
