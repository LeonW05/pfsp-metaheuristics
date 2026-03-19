#pragma once
#include <vector>
#include "individual.hpp"
using namespace std;

class Population {
    public:
        vector<Individual> individuals;
        Population(int population_size, int n_jobs, Instance& instance);
        Individual tournament(int N);
};