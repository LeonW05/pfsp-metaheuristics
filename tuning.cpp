#include <iostream>
#include <fstream>
#include <climits>
#include <cmath>
#include <random>
#include "instance.hpp"
#include "individual.hpp"
#include "population.hpp"
using namespace std;

enum CrossoverType { OX, PMX };
enum MutationType { SWAP, REVERSE };

struct Results {
    int best;
    int worst;
    float avg;
    float std;
};

Results evolvingAlg(Instance& instance, int pop_size, int gen, float px, float pm, int tournament_size, CrossoverType crossover_type = OX, MutationType mutation_type = SWAP) {
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
            
            // Saving the best individual of the current generation (elitism)
            Individual best_in_gen = pop.individuals[0];
            for (int k = 1; k < pop_size; k++) {
                if (pop.individuals[k].fitness < best_in_gen.fitness) {
                    best_in_gen = pop.individuals[k];
                }
            }
            new_population.push_back(best_in_gen);
            if (best_in_gen.fitness < best_run.fitness) {
                best_run = best_in_gen;
            }
            
            // Generating children (starting from j=1 because j=0 is the best individual we just saved)
            for (int j = 1; j < pop_size; j++) {
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
    return {best, worst, avg, std};
}

void tuningEA(Instance& instance) {
    ofstream csv("./results/tuning_ea.csv");
    csv << "parameter,value,best,worst,avg,std" << endl;

    cout << "\n--- Tuning Population Size ---" << endl;
    vector<int> pop_sizes = {50, 100, 200};
    for (int pop_size : pop_sizes) {
        int gen = 10000 / pop_size;
        cout << "pop_size=" << pop_size << " gen=" << gen << ": ";
        Results r = evolvingAlg(instance, pop_size, gen, 0.7, 0.1, 5);
        csv << "pop_size," << pop_size << "," << r.best << "," << r.worst << "," << r.avg << "," << r.std << endl;
    }

    cout << "\n--- Tuning Crossover Probability ---" << endl;
    vector<float> px_values = {0.5, 0.7, 0.9};
    for (float px : px_values) {
        cout << "px=" << px << ": ";
        Results r = evolvingAlg(instance, 100, 100, px, 0.1, 5);
        csv << "px," << px << "," << r.best << "," << r.worst << "," << r.avg << "," << r.std << endl;
    }

    cout << "\n--- Tuning Mutation Probability ---" << endl;
    vector<float> pm_values = {0.05, 0.1, 0.2};
    for (float pm : pm_values) {
        cout << "pm=" << pm << ": ";
        Results r = evolvingAlg(instance, 100, 100, 0.7, pm, 5);
        csv << "pm," << pm << "," << r.best << "," << r.worst << "," << r.avg << "," << r.std << endl;
    }

    cout << "\n--- Tuning Crossover Type ---" << endl;
    cout << "OX: ";
    Results r_ox = evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, OX);
    csv << "crossover,OX," << r_ox.best << "," << r_ox.worst << "," << r_ox.avg << "," << r_ox.std << endl;
    cout << "PMX: ";
    Results r_pmx = evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, PMX);
    csv << "crossover,PMX," << r_pmx.best << "," << r_pmx.worst << "," << r_pmx.avg << "," << r_pmx.std << endl;

    cout << "\n--- Tuning Mutation Type ---" << endl;
    cout << "Swap: ";
    Results r_swap = evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, OX, SWAP);
    csv << "mutation,Swap," << r_swap.best << "," << r_swap.worst << "," << r_swap.avg << "," << r_swap.std << endl;
    cout << "Reverse: ";
    Results r_rev = evolvingAlg(instance, 100, 100, 0.7, 0.1, 5, OX, REVERSE);
    csv << "mutation,Reverse," << r_rev.best << "," << r_rev.worst << "," << r_rev.avg << "," << r_rev.std << endl;
    
    csv.close();
}

Results simulatedAnnealingAlg(Instance& instance, float T0, float alpha, float T_min, int max_evals = 10000) {
    random_device rd;
    mt19937 g(rd());
    uniform_real_distribution<float> dis(0.0, 1.0);
    
    int best = INT_MAX;
    int worst = -1;
    float avg = 0;
    float std = 0;
    vector<int> fitnesses;
    
    for (int run = 0; run < 10; run++) {
        Individual current(instance.n_jobs, instance);
        Individual best_run = current;
        float T = T0;
        int evals = 1;
        
        while (T > T_min && evals < max_evals) {
            Individual neighbor = current;
            neighbor.mutationSwap(instance);
            evals++; 
            
            if (neighbor.fitness < current.fitness) {
                current = neighbor;
                if (current.fitness < best_run.fitness) {
                    best_run = current;
                }
            } else {
                float acceptance_prob = exp((current.fitness - neighbor.fitness) / T);
                if (dis(g) < acceptance_prob) {
                    current = neighbor;
                }
            }
            T *= alpha;
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
    
    cout << "SA Best: " << best << " Worst: " << worst << " Avg: " << avg << " Std: " << std << endl;
    return {best, worst, avg, std};
}

void tuningSA(Instance& instance) {
    ofstream csv("./results/tuning_sa.csv");
    csv << "parameter,value,best,worst,avg,std" << endl;

    cout << "\n--- Tuning SA: Initial Temperature (T0) ---" << endl;
    vector<float> T0_values = {100.0, 1000.0, 10000.0};
    for (float T0 : T0_values) {
        cout << "T0=" << T0 << ": ";
        Results r = simulatedAnnealingAlg(instance, T0, 0.999, 0.01, 10000);
        csv << "T0," << T0 << "," << r.best << "," << r.worst << "," << r.avg << "," << r.std << endl;
    }

    cout << "\n--- Tuning SA: Cooling Rate (alpha) ---" << endl;
    vector<float> alpha_values = {0.90, 0.95, 0.99, 0.999};
    for (float alpha : alpha_values) {
        cout << "alpha=" << alpha << ": ";
        Results r = simulatedAnnealingAlg(instance, 1000.0, alpha, 0.01, 10000);
        csv << "alpha," << alpha << "," << r.best << "," << r.worst << "," << r.avg << "," << r.std << endl;
    }
    
    csv.close();
}

int main() {
    Instance instance;
    instance.loadFile("./instances/tai20_5_0.fsp");

    tuningEA(instance);
    tuningSA(instance);

    return 0;
}