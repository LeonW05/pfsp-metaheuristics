#include <iostream>
#include "instance.hpp"
using namespace std;


int main() {
    Instance instance;

    instance.loadFile("/home/leon/PWR/Optimization Methods/pfsp-metaheuristics/instances/tai20_5_0.fsp");
    int res = instance.evaluate({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
    cout << res << endl;
}