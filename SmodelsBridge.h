#ifndef SMODELSBRIDGE_H
#define SMODELSBRIDGE_H

#include "Bridge.h"

class SmodelsBridge: public Bridge {
public:
    unordered_set<Literal> getAnswerSet(const Program &p) { /*stub*/ throw false; };
    virtual bool existsAnswerSet(const Program &p)  { /*stub*/ return false; } ;
private:

};

#endif //SMODELSBRIDGE_H
