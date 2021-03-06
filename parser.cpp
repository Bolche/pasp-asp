#include "parser.h"

using namespace std;

void fatal(const char* msg) {
    cerr << msg << endl;
    exit(1);
}


inline void parseBasicRule(Program &p, istream &in)  {
    Literal head;
    int bodySize, negativeBodySize;
    in >> head >> bodySize >> negativeBodySize;
    vector<Literal> negativeBody(negativeBodySize), positiveBody(bodySize - negativeBodySize);
    
    for (int i = 0; i < negativeBodySize; i++)
        in >> negativeBody[i];
    for (int i = 0; i < bodySize - negativeBodySize; i++)
        in >> positiveBody[i];
    
    p.addRule(make_shared<BasicRule>(head, positiveBody, negativeBody));
}

inline void parseChoiceRule(Program &p, istream &in) {
    int headSize, bodySize, negativeBodySize;
    in >> headSize;
    vector<Literal> headVec(headSize);
    for (int i = 0; i < headSize; i++)
        in >> headVec[i];
    
    in >> bodySize >> negativeBodySize;
    vector<Literal> negativeBody(negativeBodySize), positiveBody(bodySize - negativeBodySize);
    
    for (int i = 0; i < negativeBodySize; i++)
        in >> negativeBody[i];
    for (int i = 0; i < bodySize - negativeBodySize; i++)
        in >> positiveBody[i];
    
    p.addRule(make_shared<ChoiceRule>(headVec, positiveBody, negativeBody));
}

inline void parseRules(Program &p, istream &in) {
    int ruleType;
    
    in >> ruleType;
    while (ruleType != 0) {
        switch (ruleType) {
        case 1:
            parseBasicRule(p, in);
            break;
        case 3:
            parseChoiceRule(p, in);
            break;
        default:
            fatal("unsupported rule type");
        }
        in >> ruleType;
    }

}

inline void parseLiteralNames(Program &p, istream &in) {
    Literal l;
    in >> l;
    while (l != 0) {
        string name;
        in >> name;
        p.nameLiteral(l, name);
        in >> l;
    }
}

inline void parsePositiveTargets(Program &p, istream &in) {
    string token;
    Literal l;
    in >> token;
    if (token != "B+")
        fatal("Parser error: expected B+");

    in >> l;    
    while (l != 0) {
        p.positiveTargets.push_back(l);
        in >> l;
    }
}

inline void parseNegativeTargets(Program &p, istream &in) {
    string token;
    Literal l;
    in >> token;
    if (token != "B-")
        fatal("Parser error: expected B-");

    in >> l;
    while (l != 0) {
        p.negativeTargets.push_back(l);
        in >> l;
    }
}

void parseGroundedASP(Program &p, istream &in) {
    parseRules(p, in);
    parseLiteralNames(p, in);
    parsePositiveTargets(p, in);
    parseNegativeTargets(p, in);
}

void parseProbabilities(Program &p, istream &in) {
    while (in.good()) {
        string literal;
        double probability;
        in >> literal >> probability;
        if (literal != "")
            p.assignProbability(literal, probability);
    }
}
