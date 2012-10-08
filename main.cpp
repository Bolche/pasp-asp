#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Program.h"
#include "SmodelsBridge.h"
#include "parser.h"

using namespace std;

int main (int argc, char** argv) {
    SmodelsBridge sb;
    Program p(&sb);

    if (argc < 2)
        fatal("Usage: pasp-asp [file]");
    
    parseGroundedASP(p, cin);
    ifstream probabilitiesFile (argv[1]);
    parseProbabilities(p, probabilitiesFile);

    pair<Eigen::MatrixXd, Eigen::VectorXd> solution;
    try {
        solution = p.solve();
        cout << "ANSWER: CONSISTENT" << endl;
        cout << "Base:" << endl << solution.first << endl << endl << "Probability assignment:" << endl << solution.second << endl;
    } catch (bool e) {
        cout << "The program and the probabilities are inconsistent. There is no solution." << endl;
        cout << "ANSWER: INCONSISTENT" << endl;
    }
    
    return 0;
}
