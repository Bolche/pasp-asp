/*
 * File:   ProgramTest.cpp
 * Author: eduardo
 *
 * Created on 08/04/2012, 18:13:56
 */

#include <memory>
#include <Eigen/LU>
#include <iostream>
#include "ProgramTest.h"
#include "assertListEqual.h"

#include "MockBridge.h"
//#include "assertListEqual.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ProgramTest);

MockBridge mock(std::vector<std::unordered_set<Literal>>({{}}));
Program p1(&mock);

ProgramTest::ProgramTest() {
}

ProgramTest::~ProgramTest() {
}

void ProgramTest::setUp() {
    p1.addRule(make_shared<ChoiceRule>(vector<Literal>({1,2}),vector<Literal>({}),vector<Literal>({})));
    p1.addRule(make_shared<ChoiceRule>(vector<Literal>({3,4}),vector<Literal>({1}),vector<Literal>({})));
    p1.addRule(make_shared<BasicRule>(3,vector<Literal>({2}),vector<Literal>({})));
    p1.addRule(make_shared<BasicRule>(4,vector<Literal>({2}),vector<Literal>({})));
    p1.nameLiteral(1, "a");
    p1.nameLiteral(2, "-a");
    p1.nameLiteral(3, "b");
    p1.nameLiteral(4, "c");
    p1.assignProbability("b", 0.7);
    p1.assignProbability("c", 0.4);
}

void ProgramTest::tearDown() {
}

void ProgramTest::testConsistentWithPartialAS() {
    mock.setNewAnswerSetSet({ {0}, {1, 3}, {2, 3, 4}});
    CPPUNIT_ASSERT(! p1.consistent( Eigen::Vector3d({1, 0, 0}) ));
    CPPUNIT_ASSERT(p1.consistent( Eigen::Vector3d({1, 1, 0}) ));
    CPPUNIT_ASSERT(p1.consistent( Eigen::Vector3d({1, 1, 1}) ));

    Rule_ptr generatedConstraints = *(--mock.spyArguments(0).rulesEndIterator());
    CPPUNIT_ASSERT_LIST_EQUALS(vector<Literal>({3, 4}), dynamic_cast<ConstraintRule *>(generatedConstraints.get())->positiveBody);

    generatedConstraints = *(--mock.spyArguments(1).rulesEndIterator());
    CPPUNIT_ASSERT_LIST_EQUALS(vector<Literal>({4}), dynamic_cast<ConstraintRule *>(generatedConstraints.get())->positiveBody);
    CPPUNIT_ASSERT_LIST_EQUALS(vector<Literal>({3}), dynamic_cast<ConstraintRule *>(generatedConstraints.get())->negativeBody);
    
    generatedConstraints = *(--mock.spyArguments(2).rulesEndIterator());
    CPPUNIT_ASSERT_LIST_EQUALS(vector<Literal>({3, 4}), dynamic_cast<ConstraintRule *>(generatedConstraints.get())->negativeBody);
}

void ProgramTest::testChangeBase() {
    Eigen::MatrixXd m(3,3);
    m<< 1, 1, 1,
        0, 1, 1,
        0, 0, 1;
    Eigen::MatrixXd inverseM = m.inverse();
    Eigen::VectorXd costs(3); costs << 1,0,0;
    p1.changeBase(m, inverseM, Eigen::Vector3d(1,0,1), Eigen::Vector3d(0.3,0.3,0.4), costs);
    
    Eigen::VectorXd expectedCost(3); expectedCost << 0,0,0;
    CPPUNIT_ASSERT_EQUAL(expectedCost, costs);
    Eigen::MatrixXd expectedBase(3,3);
    expectedBase << 1, 1, 1,
        0, 1, 1,
        1, 0, 1;
    CPPUNIT_ASSERT_EQUAL(expectedBase, m);
    Eigen::MatrixXd expectedInverse = expectedBase.inverse();
    CPPUNIT_ASSERT_EQUAL(expectedInverse, inverseM);
}

void ProgramTest::testAnswerSetToBase() {
    Eigen::VectorXd expected(3);
    expected << 1, 0, 0;
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({2})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1, 2})) );
    expected << 1, 1, 0;
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({3})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1, 3})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({2, 3})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1, 2, 3})) );
    expected << 1, 0, 1;
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({4})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1, 4})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({2, 4})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1, 2, 4})) );
    expected << 1, 1, 1;
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({3, 4})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1, 3, 4})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({2, 3, 4})) );
    CPPUNIT_ASSERT_EQUAL(expected, p1.answerSetToVector(unordered_set<Literal>({1, 2, 3, 4})) );
}

void ProgramTest::testSelectColumn() {
    mock.setNewAnswerSetSet({ {1, 4} });
    Eigen::MatrixXd base(3,3);
    base<< 1, 1, 1,
        0, 1, 1,
        0, 0, 1;
    Eigen::MatrixXd inverseBase = base.inverse();
    Eigen::VectorXd costs(3); costs << 1,0,0;

    Eigen::VectorXd expected(3); expected << 1, 0, 1;
    CPPUNIT_ASSERT_EQUAL(expected, p1.selectColumn(inverseBase, costs));

    expected[0] = 1; expected[1] = -1; expected[2] = 0;
    CPPUNIT_ASSERT_EQUAL(expected, Eigen::VectorXd(costs.transpose() * inverseBase));

    Rule_ptr lastRule = *(--mock.spyArguments(0).rulesEndIterator());
    WeightRule* generatedConstraints = dynamic_cast<WeightRule *>(lastRule.get());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1, generatedConstraints->upperLimit, 1.0E-10);
    CPPUNIT_ASSERT_EQUAL(2, (int) generatedConstraints->positiveBody.size());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1, generatedConstraints->positiveBody.at(3), 1.0E-10);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, generatedConstraints->positiveBody.at(4), 1.0E-10);

}

void ProgramTest::testSolve() {
    mock.setNewAnswerSetSet({ {0}, {1, 3}, {2, 3, 4}, {1, 4} });
    pair<Eigen::MatrixXd, Eigen::VectorXd> actual = p1.solve();

    Eigen::MatrixXd expectedBase(3,3);
    expectedBase << 1, 1, 1,
        0, 1, 1,
        1, 0, 1;
    CPPUNIT_ASSERT_EQUAL(expectedBase, actual.first);

    Eigen::ArrayXd expectedPi(3); expectedPi << 0.3, 0.6, 0.1;
    // CPPUNIT_ASSERT_EQUAL(expectedPi, actual.second), considering rounding errors
    CPPUNIT_ASSERT(((expectedPi - actual.second.array()).abs() < 1.0E-10).all());
}
