#ifndef RULE_H
#define RULE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>


/* Represents a Literal */
typedef int Literal;

struct Rule {
    enum RuleType { BASIC, CONSTRAINT, WEIGHTCONSTRAINT };
    RuleType type;
};

struct BasicRule: public Rule {
    BasicRule(const Literal h, const std::vector<Literal>& p, const std::vector<Literal>& n) : head(h), positiveBody(p), negativeBody(n), type(BASIC) {};
    Literal head;
    std::vector<Literal> positiveBody;
    std::vector<Literal> negativeBody;
    RuleType type;
};
 
struct ConstraintRule: public Rule {
    ConstraintRule(const std::vector<Literal>& p, const std::vector<Literal>& n) : positiveBody(p), negativeBody(n), type(CONSTRAINT) {};
    std::vector<Literal> positiveBody;
    std::vector<Literal> negativeBody;
    RuleType type;
};

struct WeightRule: public Rule {
    WeightRule(const std::unordered_map<Literal, double>& b, const double l) : positiveBody(b), upperLimit(l), type(WEIGHTCONSTRAINT) {};
    std::unordered_map<Literal, double> positiveBody;
    double upperLimit;
    RuleType type;
};

typedef std::shared_ptr<Rule> Rule_ptr;

#endif //RULE_H
