#include "markov.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Create and initialize Markov chain
MarkovChain* markov_create(int n_states) {
    MarkovChain *chain = (MarkovChain*)malloc(sizeof(MarkovChain));
    chain->n_states = n_states;
    
    // Allocate matrix
    chain->matrix = (double**)malloc(n_states * sizeof(double*));
    for (int i = 0; i < n_states; i++) {
        chain->matrix[i] = (double*)calloc(n_states, sizeof(double));
    }
    
    // Allocate steady state vector
    chain->steady_state = (double*)calloc(n_states, sizeof(double));
    
    return chain;
}

// Free Markov chain memory
void markov_free(MarkovChain *chain) {
    if (chain) {
        for (int i = 0; i < chain->n_states; i++) {
            free(chain->matrix[i]);
        }
        free(chain->matrix);
        free(chain->steady_state);
        free(chain);
    }
}

// Set transition rate from state i to state j
void markov_set_rate(MarkovChain *chain, int i, int j, double rate) {
    if (i >= 0 && i < chain->n_states && j >= 0 && j < chain->n_states) {
        chain->matrix[i][j] = rate;
    }
}

// Calculate diagonal elements (sum of outgoing rates, negated)
void markov_finalize_matrix(MarkovChain *chain) {
    for (int i = 0; i < chain->n_states; i++) {
        double sum = 0.0;
        for (int j = 0; j < chain->n_states; j++) {
            if (i != j) {
                sum += chain->matrix[i][j];
            }
        }
        chain->matrix[i][i] = -sum;
    }
}

// Print the transition matrix
void markov_print_matrix(MarkovChain *chain) {
    printf("\nTransition Rate Matrix Q:\n");
    printf("      ");
    for (int j = 0; j < chain->n_states; j++) {
        printf("    S%-2d   ", j);
    }
    printf("\n");
    
    for (int i = 0; i < chain->n_states; i++) {
        printf("S%-2d  ", i);
        for (int j = 0; j < chain->n_states; j++) {
            printf("%9.5f ", chain->matrix[i][j]);
        }
        printf("\n");
    }
}

// Print steady-state probabilities
void markov_print_steady_state(MarkovChain *chain) {
    printf("\nSteady-State Probabilities:\n");
    for (int i = 0; i < chain->n_states; i++) {
        printf("p%-2d = %.6f\n", i, chain->steady_state[i]);
    }
    
    // Check sum
    double sum = 0.0;
    for (int i = 0; i < chain->n_states; i++) {
        sum += chain->steady_state[i];
    }
    printf("\nSum of probabilities: %.10f\n", sum);
}

// Get steady-state probability for state i
double markov_get_probability(MarkovChain *chain, int state) {
    if (state >= 0 && state < chain->n_states) {
        return chain->steady_state[state];
    }
    return 0.0;
}

// Gaussian elimination to solve linear system
// Solves Ax = b, where A is n x n and b is n x 1
static int gaussian_elimination(double **A, double *b, double *x, int n) {
    const double EPSILON = 1e-10;
    
    // Create augmented matrix
    double **aug = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        aug[i] = (double*)malloc((n + 1) * sizeof(double));
        for (int j = 0; j < n; j++) {
            aug[i][j] = A[i][j];
        }
        aug[i][n] = b[i];
    }
    
    // Forward elimination
    for (int i = 0; i < n; i++) {
        // Find pivot
        int max_row = i;
        for (int k = i + 1; k < n; k++) {
            if (fabs(aug[k][i]) > fabs(aug[max_row][i])) {
                max_row = k;
            }
        }
        
        // Swap rows
        if (max_row != i) {
            double *temp = aug[i];
            aug[i] = aug[max_row];
            aug[max_row] = temp;
        }
        
        // Check for singular matrix
        if (fabs(aug[i][i]) < EPSILON) {
            // Try to continue
            continue;
        }
        
        // Eliminate column
        for (int k = i + 1; k < n; k++) {
            double factor = aug[k][i] / aug[i][i];
            for (int j = i; j <= n; j++) {
                aug[k][j] -= factor * aug[i][j];
            }
        }
    }
    
    // Back substitution
    for (int i = n - 1; i >= 0; i--) {
        if (fabs(aug[i][i]) < EPSILON) {
            x[i] = 0.0;
            continue;
        }
        
        x[i] = aug[i][n];
        for (int j = i + 1; j < n; j++) {
            x[i] -= aug[i][j] * x[j];
        }
        x[i] /= aug[i][i];
    }
    
    // Free augmented matrix
    for (int i = 0; i < n; i++) {
        free(aug[i]);
    }
    free(aug);
    
    return 0;
}

// Solve for steady-state probabilities
// We solve Q*π = 0 with constraint Σπ_i = 1
// Replace last equation with normalization constraint
int markov_solve_steady_state(MarkovChain *chain) {
    int n = chain->n_states;
    
    // Create copy of matrix for solving
    double **A = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        A[i] = (double*)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            A[i][j] = chain->matrix[j][i]; // Transpose for π*Q = 0
        }
    }
    
    // Replace last row with normalization constraint: Σπ_i = 1
    for (int j = 0; j < n; j++) {
        A[n-1][j] = 1.0;
    }
    
    // Create b vector (all zeros except last element = 1)
    double *b = (double*)calloc(n, sizeof(double));
    b[n-1] = 1.0;
    
    // Solve the system
    int result = gaussian_elimination(A, b, chain->steady_state, n);
    
    // Free temporary arrays
    for (int i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);
    free(b);
    
    return result;
}
