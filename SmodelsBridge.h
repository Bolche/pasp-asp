#ifndef SMODELSBRIDGE_H
#define SMODELSBRIDGE_H

#include "Bridge.h"

class SmodelsBridge: public Bridge {
public:
    std::unordered_set<Literal> getAnswerSet(const Program &p);
    bool existsAnswerSet(const Program &p);
private:
    void createSmodelsProgram(stringstream &smodelsProgram, const Program &p) const;
    void createSmodelsRule(stringstream &, const BasicRule *) const;
    void createSmodelsRule(stringstream &, const ChoiceRule *) const;
    void createSmodelsRule(stringstream &, const WeightRule *) const;
    void createSmodelsRule(stringstream &, const ConstraintRule *) const;
};

#endif //SMODELSBRIDGE_H
