#include "instance.hpp"
#include <fstream>
#include <iostream>

using namespace std;

void skipLine(ifstream& file) {
    char junk = ' ';
    while (junk != '\n') {
        file.get(junk);
    }
}

void Instance::loadFile(string filename) {
    ifstream file(filename);

    skipLine(file);
    
    file >> n_jobs >> n_machines;

    // Skip to line 3 (The Matrix)
    skipLine(file);
    skipLine(file);

    processing_times.resize(n_machines, vector<int>(n_jobs));

    for (int i = 0; i < n_machines; i++) {
        for (int j = 0; j < n_jobs; j++) {
            int buffer;
            file >> buffer;
            processing_times[i][j] = buffer;
        }
    }
}