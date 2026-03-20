#include "instance.hpp"
#include "individual.hpp"
#include <algorithm>
#include <random>
using namespace std;

Individual::Individual(int n_jobs, Instance& instance) {
    sequence.resize(n_jobs);
    for (int i = 0; i < n_jobs; i++) {
        sequence[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(sequence.begin(), sequence.end(), g);

    fitness = instance.evaluate(sequence);
}

void Individual::mutationSwap(Instance& instance) {
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<> dis(0, sequence.size() - 1);
    int i = dis(g);
    int j = dis(g);
    while (j == i) {
        j = dis(g);
    }
    swap(sequence[i], sequence[j]);
    fitness = instance.evaluate(sequence);
}

void Individual::mutationReverse(Instance& instance) {
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<> dis(0, sequence.size() - 1);
    int i = dis(g);
    int j = dis(g);
    while (j == i) {
        j = dis(g);
    }
    if (i > j) {
        swap(i, j);
    }
    reverse(sequence.begin() + i, sequence.begin() + j + 1);
    fitness = instance.evaluate(sequence);
}

Individual Individual::crossoverOX (Individual& p2, Instance& instance) {
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<> dis(0, sequence.size() - 1);
    int i = dis(g);
    int j = dis(g);
    while (j == i) {
        j = dis(g);
    }
    if (i > j) {
        swap(i, j);
    }
    
    Individual child = *this;

    for (int k = i; k <= j; k++) {
        child.sequence[k] = sequence[k];
    }
    int current_index = j + 1;
    if (current_index >= sequence.size()) {
        current_index = 0;
    }
    for (int k = 0; k < sequence.size(); k++) {
        if (find(child.sequence.begin() + i, child.sequence.begin() + j + 1, p2.sequence[k]) == child.sequence.begin() + j + 1) {
            if (current_index >= sequence.size()) {
                current_index = 0;
            }

            child.sequence[current_index] = p2.sequence[k];
            current_index++;
        }
    }
    child.fitness = instance.evaluate(child.sequence);
    return child;
}

pair <Individual, Individual> Individual::crossoverPMX(Individual& p2, Instance& instance) {
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<> dis(0, sequence.size() - 1);
    int i = dis(g);
    int j = dis(g);
    while (j == i) {
        j = dis(g);
    }
    if (i > j) {
        swap(i, j);
    }

    Individual child1 = *this;
    Individual child2 = p2;

    for (int k = i; k <= j; k++) {
        swap(child1.sequence[k], child2.sequence[k]);
    }

    for (int k = i; k <= j; k++) {
        for (int l = 0; l < sequence.size(); l++) {
            if (l >= i && l <= j) {
                continue;
            }
            if (child1.sequence[k] == child1.sequence[l]) {
                child1.sequence[l] = child2.sequence[k];
            }
        }
    }

    for (int k = 0; k < sequence.size(); k++) {
        if (k >= i && k <= j) {
            continue;
        }
        while (find(child1.sequence.begin() + i, child1.sequence.begin() + j + 1, child1.sequence[k]) != child1.sequence.begin() + j + 1) {
            int pos = find(p2.sequence.begin() + i, p2.sequence.begin() + j + 1, child1.sequence[k]) - p2.sequence.begin();
            child1.sequence[k] = sequence[pos];
        }        
    }

    for (int k = 0; k < sequence.size(); k++) {
        if (k >= i && k <= j) {
            continue;
        }
        while (find(child2.sequence.begin() + i, child2.sequence.begin() + j + 1, child2.sequence[k]) != child2.sequence.begin() + j + 1) {
            int pos = find(sequence.begin() + i, sequence.begin() + j + 1, child2.sequence[k]) - sequence.begin();
            child2.sequence[k] = p2.sequence[pos];
        }        
    }


    return {child1, child2};
}
