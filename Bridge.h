#ifndef BRIDGE_H
#define BRIDGE_H

#include <unordered_set>
#include "Rule.h"
#include "Program.h"

class Bridge {
public:
    virtual std::unordered_set<Literal> getAnswerSet(const Program &p) = 0;
    virtual bool existsAnswerSet(const Program &p) = 0;
};

#endif //BRIDGE_H
