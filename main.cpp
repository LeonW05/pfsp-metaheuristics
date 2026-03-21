#include <iostream>
#include <set>
#include "instance.hpp"
#include "individual.hpp"
#include "population.hpp"
#include "greedy.hpp"
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

    pair<Individual, Individual> children = p1.crossoverPMX(p2, instance);
    cout << "\nChild 1: ";
    for (int x : children.first.sequence) cout << x << " ";
    cout << "\nChild 1 fitness: " << children.first.fitness << endl;
    cout << "Child 2: ";
    for (int x : children.second.sequence) cout << x << " ";
    cout << "\nChild 2 fitness: " << children.second.fitness << endl;

    set<int> check(children.first.sequence.begin(), children.first.sequence.end());
    cout << "Child1 unique values: " << check.size() << endl;
    set<int> check2(children.second.sequence.begin(), children.second.sequence.end());
    cout << "Child2 unique values: " << check2.size() << endl;

    Individual greedy_solution = GreedyAlg().greedy(instance);
    cout << "\nGreedy solution: ";
    for (int x : greedy_solution.sequence) cout << x << " ";
    cout << "\nGreedy fitness: " << greedy_solution.fitness << endl;

}