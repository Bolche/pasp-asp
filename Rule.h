#ifndef RULE_H
#define RULE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>


/* Represents a Literal */
typedef int Literal;

struct Rule {
    enum RuleType { BASIC, CHOICE, CONSTRAINT, WEIGHTCONSTRAINT };
    virtual RuleType getType() const = 0;
};

struct BasicRule: public Rule {
BasicRule(const Literal h, const std::vector<Literal>& p, const std::vector<Literal>& n) : head(h), positiveBody(p), negativeBody(n) {};
    RuleType getType() const {return BASIC;};
    Literal head;
    std::vector<Literal> positiveBody;
    std::vector<Literal> negativeBody;
};

struct ChoiceRule: public Rule {
ChoiceRule(const std::vector<Literal>& h, const std::vector<Literal>& p, const std::vector<Literal>& n) : head(h), positiveBody(p), negativeBody(n) {};
    RuleType getType() const {return CHOICE;};
    std::vector<Literal> head;
    std::vector<Literal> positiveBody;
    std::vector<Literal> negativeBody;
};

struct ConstraintRule: public Rule {
ConstraintRule(const std::vector<Literal>& p, const std::vector<Literal>& n) : positiveBody(p), negativeBody(n) {};
    RuleType getType() const {return CONSTRAINT;};
    std::vector<Literal> positiveBody;
    std::vector<Literal> negativeBody;
};

struct WeightRule: public Rule {
WeightRule(const std::unordered_map<Literal, double>& b, const double l) : positiveBody(b), upperLimit(l) {};
    RuleType getType() const {return WEIGHTCONSTRAINT;};
    std::unordered_map<Literal, double> positiveBody;
    double upperLimit;
};

typedef std::shared_ptr<Rule> Rule_ptr;

#endif //RULE_H
