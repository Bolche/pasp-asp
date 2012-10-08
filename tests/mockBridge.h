#ifndef MOCKBRIDGE_H
#define MOCKBRIDGE_H

#include <vector>
#include "Bridge.h"

class MockBridge : public Bridge {
public:
    MockBridge(const vector<unordered_set<Literal>> &as) : answerSets(as), responseCount(0) {};
    unordered_set<Literal> getAnswerSet(const Program &p);
    bool existsAnswerSet(const Program &p);
    void setNewAnswerSetSet(const vector<unordered_set<Literal>> &as);
    Program spyArguments(unsigned int) const;
private:
    vector<Program> arguments;
    vector<unordered_set<Literal>> answerSets;
    unsigned int responseCount;
};

#endif //MOCKBRIDGE_H
