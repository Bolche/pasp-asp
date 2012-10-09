#include <sstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>

#include "SmodelsBridge.h"
#include "c++11_warning.h"

using namespace std;
unordered_set<Literal> SmodelsBridge::getAnswerSet(const Program &p) {
    stringstream smodelsProgram, smodelsResponseBuf;
    createSmodelsProgram(smodelsProgram, p);

    string programString = smodelsProgram.str();
#ifdef PRINT_DEBUG
    cout << "DEBUG:\n" << programString;
#endif
    
    pid_t pid;
    int pipefd[2];
    char buf[256];

    pipe(pipefd);
    pid = fork();
    if (pid == 0) {
        // Child
        dup2(pipefd[0], STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        execl(smodelsExecPath, (char*) NULL);
        exit(1);
    }

    write(pipefd[0], programString.c_str(), programString.size()); // write message to the process
    while (read(pipefd[1], buf, sizeof(buf))) {
        smodelsResponseBuf << buf;
    }

#ifdef PRINT_DEBUG
    cout << "DEBUG:\n" << smodelsResponseBuf;
#endif
    
    unordered_set<Literal> answerSet;
    string line;
    getline(smodelsResponseBuf, line); //Smodels version
    getline(smodelsResponseBuf, line);
    if (line == "False")
        throw false;
    else if (line == "Answer: 1") {
        getline(smodelsResponseBuf, line);
        stringstream asStream(line);
        string atom;
        smodelsResponseBuf >> atom;
        smodelsResponseBuf >> atom;
        while (!smodelsResponseBuf.eof()) {
            answerSet.insert(nameToLiteral(atom.c_str()));
            smodelsResponseBuf >> atom;
        }   
    } else {
        cerr << "Error parsing smodels output" << endl;
        exit(1);
    }

    return answerSet;
}

Literal SmodelsBridge::nameToLiteral(const char *n) const {
    return atoi(n+1); //Skip one character and convert to integer
}

bool SmodelsBridge::existsAnswerSet(const Program &p) {
    try {
        getAnswerSet(p);
        return true;
    } catch (bool e) {
        return false;
    }
}

void SmodelsBridge::createSmodelsProgram(stringstream &smodelsProgram, const Program &p) {
    countLiterals(p);
    for (auto rule = p.rulesBeginIterator(); rule != p.rulesEndIterator(); rule++) {
        /* Ugly code. Conversion should be implicit. */
        switch ((*rule)->getType()) {
        case Rule::RuleType::BASIC:
            createSmodelsRule(smodelsProgram, dynamic_cast<BasicRule *>(rule->get()));
            break;
        case Rule::RuleType::CHOICE:
            createSmodelsRule(smodelsProgram, dynamic_cast<ChoiceRule *>(rule->get()));
            break;
        case Rule::RuleType::CONSTRAINT:
            createSmodelsRule(smodelsProgram, dynamic_cast<ConstraintRule *>(rule->get()));
            break;
        case Rule::RuleType::WEIGHTCONSTRAINT:
            createSmodelsRule(smodelsProgram, dynamic_cast<WeightRule *>(rule->get()));
            break;
        }
    }

    smodelsProgram << "0" << endl;
    for (int i = 2; i< maxLiteral; i++)
        smodelsProgram << i << ' ' << 'a' << i << endl; 
    smodelsProgram << "0" << endl;
    smodelsProgram << "B+" << endl;
    for (auto it = p.positiveTargets.begin(); it != p.positiveTargets.end(); it++)
        smodelsProgram << *it << endl;
    smodelsProgram << "0" << endl;
    smodelsProgram << "B-" << endl;
    for (auto it = p.negativeTargets.begin(); it != p.negativeTargets.end(); it++)
        smodelsProgram << *it << endl;
    smodelsProgram << "0" << endl;
    smodelsProgram << "1" << endl;
}

void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const BasicRule *rule) {
    smodelsProgram << "1 " << rule->head << ' ' << (rule->positiveBody.size() + rule->negativeBody.size()) << ' ' << rule->negativeBody.size();
    for (auto it = rule->negativeBody.begin(); it != rule->negativeBody.end(); it++)
        smodelsProgram << ' ' << *it;
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++)
        smodelsProgram << ' ' << *it;
    smodelsProgram << endl;
}
void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const ChoiceRule *rule) {
    smodelsProgram << "3 " << rule->head.size();
    for (auto it = rule->head.begin(); it != rule->head.end(); it++)
        smodelsProgram << ' ' << *it;
    smodelsProgram << (rule->positiveBody.size() + rule->negativeBody.size()) << ' ' << rule->negativeBody.size();
    for (auto it = rule->negativeBody.begin(); it != rule->negativeBody.end(); it++)
        smodelsProgram << ' ' << *it;
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++)
        smodelsProgram << ' ' << *it;
    smodelsProgram << endl;
}
void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const ConstraintRule *rule) {
    smodelsProgram << "1 1 " << (rule->positiveBody.size() + rule->negativeBody.size()) << ' ' << rule->negativeBody.size();
    for (auto it = rule->negativeBody.begin(); it != rule->negativeBody.end(); it++)
        smodelsProgram << ' ' << *it;
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++)
        smodelsProgram << ' ' << *it;
    smodelsProgram << endl;
}
void SmodelsBridge::createSmodelsRule(stringstream &smodelsProgram, const WeightRule *rule) {
    unsigned long negativeWeights = 0;
    stringstream positive, negative, weightsNeg, weightsPos;
    unsigned int numNegatives = 0;
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++) {
        if (it->second < 0) {
            numNegatives++;
            negative << ' ' << it->first;
            weightsNeg << ' ' << -it->second;
            negativeWeights += it->second;
        } else {
            positive << ' ' << it->first;
            weightsPos << ' ' << it->second;
        }
    }

    Literal newLit = ++maxLiteral;
    smodelsProgram << "1 1 1 1 " << newLit << endl;
    smodelsProgram << "5 " << newLit << ' ' << (rule->upperLimit + negativeWeights + 1) << ' ' <<
        rule->positiveBody.size() << ' ' << numNegatives << ' ' << negative << ' ' << positive <<
        ' ' << weightsNeg << weightsPos << endl;
}

/* TODO optimize this */ 
void SmodelsBridge::countLiterals(const Program &p) {
    for (auto rule = p.rulesBeginIterator(); rule != p.rulesEndIterator(); rule++) {
        /* Ugly code. Conversion should be implicit. */
        switch ((*rule)->getType()) {
        case Rule::RuleType::BASIC:
            countLiterals(dynamic_cast<BasicRule *>(rule->get()));
            break;
        case Rule::RuleType::CHOICE:
            countLiterals(dynamic_cast<ChoiceRule *>(rule->get()));
            break;
        case Rule::RuleType::CONSTRAINT:
            countLiterals(dynamic_cast<ConstraintRule *>(rule->get()));
            break;
        case Rule::RuleType::WEIGHTCONSTRAINT:
            countLiterals(dynamic_cast<WeightRule *>(rule->get()));
            break;
        }
    }    
}

void SmodelsBridge::countLiterals(const BasicRule *rule) {
    if (rule->head > maxLiteral) maxLiteral = rule->head;
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++)
        if (*it > maxLiteral) maxLiteral = (*it);
    for (auto it = rule->negativeBody.begin(); it != rule->negativeBody.end(); it++)
        if (*it > maxLiteral) maxLiteral = (*it);
}
void SmodelsBridge::countLiterals(const ChoiceRule *rule) {
    for (auto it = rule->head.begin(); it != rule->head.end(); it++)
        if (*it > maxLiteral) maxLiteral = (*it);
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++)
        if (*it > maxLiteral) maxLiteral = (*it);
    for (auto it = rule->negativeBody.begin(); it != rule->negativeBody.end(); it++)
        if (*it > maxLiteral) maxLiteral = (*it);
}
void SmodelsBridge::countLiterals(const ConstraintRule *rule) {
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++)
        if (*it > maxLiteral) maxLiteral = (*it);
    for (auto it = rule->negativeBody.begin(); it != rule->negativeBody.end(); it++)
        if (*it > maxLiteral) maxLiteral = (*it);
}
void SmodelsBridge::countLiterals(const WeightRule *rule) {
    for (auto it = rule->positiveBody.begin(); it != rule->positiveBody.end(); it++)
        if (it->first > maxLiteral) maxLiteral = it->first;
}
