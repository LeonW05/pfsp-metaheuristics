#pragma once
#include "instance.hpp"
#include "individual.hpp"
#include <vector>
using namespace std;

class Individual {
    public:
        vector<int> sequence;
        int fitness;
        Individual(int n_jobs, Instance& instance);
        void mutationSwap(Instance& instance);
        void mutationReverse(Instance& instance);
        Individual crossoverOX(Individual& p2, Instance& instance);
        pair<Individual, Individual> crossoverPMX(Individual& p2, Instance& instance);
};