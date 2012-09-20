#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <cstdlib>
#include "Program.h"

void fatal(const char*) __attribute__((noreturn));
void parseGroundedASP(Program&, istream&);
void parseProbabilities(Program&, istream&);

#endif //PARSER_H
