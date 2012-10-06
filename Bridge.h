#ifndef BRIDGE_H
#define BRIDGE_H

#include <unordered_set>
#include "Program.h"

class Bridge {
public:
    unordered_set<Literal> getAnswerSet(const Program &p);
    bool existsAnswerSet(const Program &p);
};

#endif //BRIDGE_H
