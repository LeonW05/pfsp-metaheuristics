#include <iostream>
#include <fstream>
#include <set>
#include <climits>
#include <cmath>
#include <random>
#include "instance.hpp"
#include "individual.hpp"
#include "population.hpp"
#include "greedy.hpp"
using namespace std;

enum CrossoverType { OX, PMX };
enum MutationType { SWAP, REVERSE };

struct Results {
    int best;
    int worst;
    float avg;
    float std;
};

Results randomAlg(Instance& instance, int N) {
    int best = INT_MAX;
    int worst = -1;
    float avg = 0;
    float std = 0;
    vector<int> fitnesses;

    for (int i = 0; i < N; i++) {
        Individual p(instance.n_jobs, instance);
        if (p.fitness < best) best = p.fitness;
        if (p.fitness > worst) worst = p.fitness;
        avg += p.fitness;
        fitnesses.push_back(p.fitness);
    }
    avg /= N;

    for (int i = 0; i < (int)fitnesses.size(); i++) {
        std += (fitnesses[i] - avg) * (fitnesses[i] - avg);
    }
    std = sqrt(std / N);

    cout << "Best: " << best << " Worst: " << worst << " Avg: " << avg << " Std: " << std << endl;
    return {best, worst, avg, std};
}

int greedyAlg(Instance& instance) {
    GreedyAlg g;
    Individual p = g.greedy(instance);
    cout << "Greedy fitness: " << p.fitness << endl;
    return p.fitness;
}

Results evolvingAlg(Instance& instance, int pop_size, int gen, float px, float pm, int tournament_size, ofstream& convergence_csv, string instance_name, CrossoverType crossover_type = OX, MutationType mutation_type = SWAP) {
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
            
            convergence_csv << run << "," << i << "," << best_run.fitness << ",EA," << instance_name << endl;
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
    
    cout << "EA Best: " << best << " Worst: " << worst << " Avg: " << avg << " Std: " << std << endl;
    return {best, worst, avg, std};
}

Results simulatedAnnealingAlg(Instance& instance, float T0, float alpha, float T_min) {
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
        
        while (T > T_min) {
            Individual neighbor = current;
            neighbor.mutationSwap(instance);
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

int main() {
    vector<string> instances = {
        "./instances/tai20_5_0.fsp",
        "./instances/tai20_10_0.fsp",
        "./instances/tai20_20_0.fsp",
        "./instances/tai100_10_0.fsp",
        "./instances/tai100_20_0.fsp",
        "./instances/tai500_20_0.fsp"
    };

    ofstream csv("./results/results.csv");
    csv << "algorithm,instance,best,worst,avg,std" << endl;

    ofstream convergence_csv("./results/convergence.csv");
    convergence_csv << "run,generation,best_fitness,algorithm,instance" << endl;

    for (int i = 0; i < (int)instances.size(); i++) {
        Instance instance;
        instance.loadFile(instances[i]);
        string instance_name = instances[i].substr(instances[i].find_last_of("/") + 1);
        cout << "Instance " << i + 1 << " (" << instances[i] << "):" << endl;
        
        Results r_random = randomAlg(instance, 10000);
        csv << "Random," << instance_name << "," << r_random.best << "," << r_random.worst << "," << r_random.avg << "," << r_random.std << endl;

        int greedy_fitness = greedyAlg(instance);
        csv << "Greedy," << instance_name << "," << greedy_fitness << "," << greedy_fitness << "," << greedy_fitness << ",0" << endl;

        Results r_ea = evolvingAlg(instance, 100, 100, 0.5, 0.2, 5, convergence_csv, instance_name);
        csv << "EA," << instance_name << "," << r_ea.best << "," << r_ea.worst << "," << r_ea.avg << "," << r_ea.std << endl;

        Results r_sa = simulatedAnnealingAlg(instance, 1000, 0.999, 0.01);
        csv << "SA," << instance_name << "," << r_sa.best << "," << r_sa.worst << "," << r_sa.avg << "," << r_sa.std << endl;

        cout << "------------------------------------------------------" << endl;
    }

    csv.close();
    convergence_csv.close();
    cout << "Results saved to results.csv and convergence.csv" << endl;

    return 0;
}