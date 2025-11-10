#ifndef MARKOV_H
#define MARKOV_H

#include <stddef.h>

// Structure to hold transition matrix
typedef struct {
    int n_states;
    double **matrix;
    double *steady_state;
} MarkovChain;

// Create and initialize Markov chain
MarkovChain* markov_create(int n_states);

// Free Markov chain memory
void markov_free(MarkovChain *chain);

// Set transition rate from state i to state j
void markov_set_rate(MarkovChain *chain, int i, int j, double rate);

// Calculate diagonal elements (sum of outgoing rates, negated)
void markov_finalize_matrix(MarkovChain *chain);

// Solve for steady-state probabilities
int markov_solve_steady_state(MarkovChain *chain);

// Print the transition matrix
void markov_print_matrix(MarkovChain *chain);

// Print steady-state probabilities
void markov_print_steady_state(MarkovChain *chain);

// Get steady-state probability for state i
double markov_get_probability(MarkovChain *chain, int state);

#endif
