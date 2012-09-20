#include <cstdlib>
#include <fstream>

#include "Program.h"
#include "LoopFormulasTranslator.h"
#include "parser.h"

using namespace std;

int main (int argc, char** argv) {
    Program p;

    if (argc < 2)
        fatal("Usage: pasp-asp [file]");
    
    parseGroundedASP(p, cin);
    ifstream probabilitiesFile (argv[1]);
    parseProbabilities(p, probabilitiesFile);

    /* PSatToMaxSAT expected that the n atoms with probabilities are numbered 1
       to n-1. We need to reassign the numbers.*/
    p.reassignLiteralNumbers();
    
    LoopFormulasTranslator translator(p);
    translator.translate();
    cout << translator.getDimacsCNF() << endl;
    
    return 0;
}
