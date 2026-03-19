#include "population.hpp"
#include "individual.hpp"
#include <random>
using namespace std;

Population::Population(int population_size, int n_jobs, Instance& instance) {
    for (int i = 0; i < population_size; i++) {
        individuals.push_back(Individual(n_jobs, instance));
    }
}

Individual Population::tournament(int N) {
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<int> dist(0, individuals.size() - 1);

    vector<Individual> tournament_individuals;
    for (int i = 0; i < N; i++) {
        int random_index = dist(g);
        tournament_individuals.push_back(individuals[random_index]);
    }

    Individual best_individual = tournament_individuals[0];
    for (int i = 1; i < tournament_individuals.size(); i++) {
        if (tournament_individuals[i].fitness < best_individual.fitness) {
            best_individual = tournament_individuals[i];
        }
    }
    return best_individual;
}