#include <sstream>

namespace Smodels {
#include <smodels/atomrule.h>
#include <smodels/api.h>
#include <smodels/smodels.h>
#include <smodels/stable.h>
}

#include "SmodelsBridge.h"
#include "c++11_warning.h"

using namespace std;
unordered_set<Literal> SmodelsBridge::getAnswerSet(const Program &p) {
    stringstream smodelsProgram;
    createSmodelsProgram(smodelsProgram, p);
}

bool SmodelsBridge::existsAnswerSet(const Program &p) {
    try {
        getAnswerSet(p);
        return true;
    } catch (bool e) {
        return false;
    }
}

void SmodelsBridge::createSmodelsProgram(stringstream &smodelsProgram, const Program &p) const {
    for (auto rule = p.rulesBeginIterator(); rule != p.rulesEndIterator(); rule++) {
        /* Ugly code. Conversion should be implicit. */
        switch ((*rule)->getType()) {
        case Rule::RuleType::BASIC:
            createSmodelsRule(smodelsProgram, dynamic_cast<BasicRule *>(rule->get()));
            break;
        case Rule::RuleType::CHOICE:
            createSmodelsRule(smodelsProgram, dynamic_cast<BasicRule *>(rule->get()));
            break;
        case Rule::RuleType::CONSTRAINT:
            createSmodelsRule(smodelsProgram, dynamic_cast<BasicRule *>(rule->get()));
            break;
        case Rule::RuleType::WEIGHTCONSTRAINT:
            createSmodelsRule(smodelsProgram, dynamic_cast<BasicRule *>(rule->get()));
            break;
        }
    }

    smodelsProgram << "0" << endl;
    // No need for naming atoms
    smodelsProgram << "0" << endl;
    smodelsProgram << "B+" << endl;
    for (auto it = Program.positiveTargets.c_begin(); it != Program.positiveTargets.c_end(); it++)
        smodelsProgram << *it << endl;
    smodelsProgram << "0" << endl;
    smodelsProgram << "B-" << endl;
    for (auto it = Program.negativeTargets.c_begin(); it != Program.negativeTargets.c_end(); it++)
        smodelsProgram << *it << endl;
    smodelsProgram << "0" << endl;
    smodelsProgram << "1" << endl;
}

void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const BasicRule *rule) const {}
void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const ChoiceRule *rule) const {}
void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const WeightRule *rule) const {}
void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const ConstraintRule *rule) const {}
