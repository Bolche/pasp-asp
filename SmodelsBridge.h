#ifndef SMODELSBRIDGE_H
#define SMODELSBRIDGE_H

#include "Bridge.h"

class SmodelsBridge: public Bridge {
public:
    SmodelsBridge(char* s) : smodelsExecPath(s) {};
    std::unordered_set<Literal> getAnswerSet(const Program &p);
    bool existsAnswerSet(const Program &p);
private:
    Literal nameToLiteral(const char *) const;
    void countLiterals(const Program &p);
    void countLiterals(const BasicRule *);
    void countLiterals(const ChoiceRule *);
    void countLiterals(const WeightRule *);
    void countLiterals(const ConstraintRule *);
    void createSmodelsProgram(stringstream &smodelsProgram, const Program &p);
    void createSmodelsRule(stringstream &, const BasicRule *);
    void createSmodelsRule(stringstream &, const ChoiceRule *);
    void createSmodelsRule(stringstream &, const WeightRule *);
    void createSmodelsRule(stringstream &, const ConstraintRule *);
    Literal maxLiteral = 0;
    char *smodelsExecPath;
};

#endif //SMODELSBRIDGE_H
