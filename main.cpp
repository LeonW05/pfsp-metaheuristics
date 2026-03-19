#include <iostream>
#include "instance.hpp"
#include "individual.hpp"
#include "population.hpp"
using namespace std;


int main() {
    Instance instance;
    string filename = getenv("HOME") + string("/PWR/Optimization Methods/pfsp-metaheuristics/instances/tai20_5_0.fsp");
    instance.loadFile(filename);
    
    Individual p1(instance.n_jobs, instance);
    Individual p2(instance.n_jobs, instance);
    
    cout << "P1: ";
    for (int x : p1.sequence) cout << x << " ";
    cout << "\nP2: ";
    for (int x : p2.sequence) cout << x << " ";
    cout << endl;
    
    cout << "P1 fitness: " << p1.fitness << endl;
    cout << "P2 fitness: " << p2.fitness << endl;

    Individual child = p1.crossoverOX(p2, instance);
    
    cout << "\nChild: ";
    for (int x : child.sequence) cout << x << " ";
    cout << "\nChild fitness: " << child.fitness << endl;
}