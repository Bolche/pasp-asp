#include <cstdlib>
#include <fstream>

#include "Program.h"
#include "SmodelsBridge.h"
#include "parser.h"

using namespace std;

int main (int argc, char** argv) {
    SmodelsBrige sb;
    Program p(&sb);

    if (argc < 2)
        fatal("Usage: pasp-asp [file]");
    
    parseGroundedASP(p, cin);
    ifstream probabilitiesFile (argv[1]);
    parseProbabilities(p, probabilitiesFile);

    p.solve();
    
    return 0;
}
