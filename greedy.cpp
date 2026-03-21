#include "greedy.hpp"
#include "individual.hpp"
#include "instance.hpp"
#include <climits>

Individual GreedyAlg::greedy(Instance& instance) {
    vector<bool> scheduled(instance.n_jobs, false);
    vector<int> sequence;
    
    for (int i = 0; i < instance.n_jobs; i++) {
        int best_job = -1;
        int best_cost = INT_MAX;
        
        for (int j = 0; j < instance.n_jobs; j++) {
            if (!scheduled[j]) {
                sequence.push_back(j);
                int cost = instance.evaluate(sequence);
                if (cost < best_cost) {
                    best_cost = cost;
                    best_job = j;
                }
                sequence.pop_back();
            }
        }
        
        sequence.push_back(best_job);
        scheduled[best_job] = true;
    }
    
    Individual ind(instance.n_jobs, instance);
    ind.sequence = sequence;
    ind.fitness = instance.evaluate(sequence);
    return ind;
}