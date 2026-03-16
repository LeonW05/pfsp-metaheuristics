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

int Instance::evaluate(vector<int> sequence) {
    int res = 0;

    vector<vector<int>> aux;
    aux.resize(n_machines, vector<int>(n_jobs));

    for (int m = 0; m < n_machines; m++) {
        for (int j = 0; j < n_jobs; j++) {
            if (m == 0 && j == 0) {
                aux[m][j] = processing_times[m][sequence[j]];
            }

            if (j == 0 && m > 0) {
                aux[m][j] = aux[m-1][j] + processing_times[m][sequence[j]];
            }

            if (j > 0 && m == 0) {
                aux[m][j] = aux[m][j-1] + processing_times[m][sequence[j]];
            }

            if (j > 0 && m > 0) {
                aux[m][j] = max(aux[m][j-1], aux[m-1][j]) + processing_times[m][sequence[j]];
            }
        }
    }

    for (int j = 0; j < n_jobs; j++) {
        res += aux[n_machines-1][j];
    }

    return res;
}