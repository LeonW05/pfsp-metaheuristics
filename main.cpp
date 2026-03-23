#include <iostream>
#include <set>
#include <climits>
#include <cmath>
#include "instance.hpp"
#include "individual.hpp"
#include "population.hpp"
#include "greedy.hpp"
using namespace std;


void randomAlg(Instance& instance, int N) {
    int best = INT_MAX;
    int worst = -1;
    float avg = 0;
    float std = 0;
    vector<int> fitnesses;

    for (int i = 0; i < N; i++) {
        Individual p(instance.n_jobs, instance);
        if (p.fitness < best) {
            best = p.fitness;
        }

        if (p.fitness > worst) {
            worst = p.fitness;
        }
        avg += p.fitness;
        fitnesses.push_back(p.fitness);
    }
    avg /= N;

    for (int i = 0; i < fitnesses.size(); i++) {
        std += (fitnesses[i] - avg) * (fitnesses[i] - avg);
    }
    std = sqrt(std / N);

    cout << "Best: " << best << " Worst: " << worst << " Avg: " << avg << " Std: " << std << endl;
}

void greedyAlg(Instance& instance) {
    GreedyAlg g;
    Individual p = g.greedy(instance);
    cout << "Greedy fitness: " << p.fitness << endl;
}

int main() {
    Instance instance;
    string filename = getenv("HOME") + string("/PWR/Optimization Methods/pfsp-metaheuristics/instances/tai20_5_0.fsp");
    instance.loadFile(filename);
    
    randomAlg(instance, 10000);    
    greedyAlg(instance);

    return 0;
}