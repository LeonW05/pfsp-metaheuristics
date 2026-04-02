#include <iostream>
#include <climits>
#include <cmath>
#include <random>
#include "instance.hpp"
#include "individual.hpp"
#include "population.hpp"
using namespace std;

enum CrossoverType { OX, PMX };
enum MutationType { SWAP, REVERSE };

void evolvingAlg(Instance& instance, int pop_size, int gen, float px, float pm, int tournament_size, CrossoverType crossover_type = OX, MutationType mutation_type = SWAP) {
    random_device rd;
    mt19937 g(rd());
    uniform_real_distribution<float> dis(0.0, 1.0);
    
    int best = INT_MAX;
    int worst = -1;
    float avg = 0;
    float std = 0;
    vector<int> fitnesses;
    
    for (int run = 0; run < 10; run++) {
        Population pop(pop_size, instance);
        Individual best_run = pop.individuals[0];
        
        for (int i = 0; i < gen; i++) {
            vector<Individual> new_population;
            for (int j = 0; j < pop_size; j++) {
                Individual parent1 = pop.tournament(tournament_size);
                Individual parent2 = pop.tournament(tournament_size);
                Individual offspring = parent1;
                
                if (dis(g) < px) {
                    if (crossover_type == OX) {
                        offspring = parent1.crossoverOX(parent2, instance);
                    } else if (crossover_type == PMX) {
                        offspring = parent1.crossoverPMX(parent2, instance).first;
                    }
                }
                
                if (dis(g) < pm) {
                    if (mutation_type == SWAP) {
                        offspring.mutationSwap(instance);
                    } else if (mutation_type == REVERSE) {
                        offspring.mutationReverse(instance);
                    }
                }
                
                if (offspring.fitness < best_run.fitness) {
                    best_run = offspring;
                }
                new_population.push_back(offspring);
            }
            pop.individuals = new_population;
        }
        
        if (best_run.fitness < best) best = best_run.fitness;
        if (best_run.fitness > worst) worst = best_run.fitness;
        avg += best_run.fitness;
        fitnesses.push_back(best_run.fitness);
    }
    
    avg /= 10;
    for (int i = 0; i < (int)fitnesses.size(); i++) {
        std += (fitnesses[i] - avg) * (fitnesses[i] - avg);
    }
    std = sqrt(std / 10);
    
    cout << "Best: " << best << " Worst: " << worst << " Avg: " << avg << " Std: " << std << endl;
}

void tuningEA(Instance& instance) {
    cout << "\n--- Tuning Population Size ---" << endl;
    vector<int> pop_sizes = {50, 100, 200};
    for (int pop_size : pop_sizes) {
        int gen = 10000 / pop_size;
        cout << "pop_size=" << pop_size << " gen=" << gen << ": ";
        evolvingAlg(instance, pop_size, gen, 0.7, 0.1, 5);
    }

    cout << "\n--- Tuning Crossover Probability ---" << endl;
    vector<float> px_values = {0.5, 0.7, 0.9};
    for (float px : px_values) {
        cout << "px=" << px << ": ";
        evolvingAlg(instance, 100, 100, px, 0.1, 5);
    }

    cout << "\n--- Tuning Mutation Probability ---" << endl;
    vector<float> pm_values = {0.05, 0.1, 0.2};
    for (float pm : pm_values) {
        cout << "pm=" << pm << ": ";
        evolvingAlg(instance, 100, 100, 0.7, pm, 5);
    }

    cout << "\n--- Tuning Crossover Type ---" << endl;
    cout << "OX: ";
    evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, OX);
    cout << "PMX: ";
    evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, PMX);

    cout << "\n--- Tuning Mutation Type ---" << endl;
    cout << "Swap: ";
    evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, OX, SWAP);
    cout << "Reverse: ";
    evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, OX, REVERSE);
}

int main() {
    Instance instance;
    instance.loadFile("./instances/tai20_5_0.fsp");
    tuningEA(instance);
    return 0;
}