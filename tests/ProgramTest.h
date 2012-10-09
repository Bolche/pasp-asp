/* 
 * File:   ProgramTest.h
 * Author: eduardo
 *
 * Created on 8 de Abril de 2012, 19:32
 */

#ifndef PROGRAMTEST_H
#define	PROGRAMTEST_H

#include <string>
#include <list>
#include <cppunit/extensions/HelperMacros.h>

#include "Program.h"

using namespace std;

class ProgramTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(ProgramTest);
    CPPUNIT_TEST(testChangeBase);
    CPPUNIT_TEST(testAnswerSetToBase);
    CPPUNIT_TEST(testSelectColumn);
    CPPUNIT_TEST(testSolve);
    CPPUNIT_TEST_SUITE_END();
public:
    ProgramTest();
    virtual ~ProgramTest();
    void setUp();
    void tearDown();

private:
    void testChangeBase();
    void testAnswerSetToBase();
    void testSelectColumn();
    void testSolve();
};

#endif	/* PROGRAMTEST_H */

