#include "c++11_warning.h"
#include "mockBridge.h"

unordered_set<Literal> MockBridge::getAnswerSet(const Program &p) {
    arguments.push_back(p);
    return answerSets[responseCount++];
}

bool MockBridge::existsAnswerSet(const Program &p) {
    arguments.push_back(p);
    bool response = (answerSets.size() > responseCount) && (answerSets[responseCount].count(0) == 0);
    responseCount++;
    return response;
}

Program MockBridge::spyArguments(unsigned int i) const {
    return arguments[i];
}

void MockBridge::setNewAnswerSetSet(const vector<unordered_set<Literal>> &as) {
    answerSets = as;
    responseCount = 0;
    arguments.clear();
}
