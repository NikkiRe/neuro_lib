#include "markov.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// System parameters
#define LAMBDA 0.7      // Arrival rate
#define B 10.0          // Mean service time
#define MU (1.0/B)      // Service rate = 0.1

// NOTE: The provided matrix uses λ1 = 0.525 and λ2 = 0.175
// This corresponds to P1 = 0.75 and P2 = 0.25 (not 0.15 as stated in problem)
// Using the values from the actual transition matrix:
#define LAMBDA_1 0.525  // Arrival rate to server 1
#define LAMBDA_2 0.175  // Arrival rate to server 2
#define P1 (LAMBDA_1/LAMBDA)  // 0.75
#define P2 (LAMBDA_2/LAMBDA)  // 0.25

// States for System 1:
// S0: 0/0/0 - empty
// S1: 1/0/0 - 1 on P1
// S2: 0/1/0 - 1 on P2
// S3: 1/1/0 - 1 on P1, 1 on P2
// S4: 0/1/1 - 1 on P2, 1 in queue
// S5: 0/1/2 - 1 on P2, 2 in queue
// S6: 1/1/1 - 1 on P1, 1 on P2, 1 in queue
// S7: 0/1/3 - 1 on P2, 3 in queue (full)
// S8: 1/1/2 - 1 on P1, 1 on P2, 2 in queue
// S9: 1/1/3 - 1 on P1, 1 on P2, 3 in queue (full)

void build_system1_matrix(MarkovChain *chain) {
    // Based on the provided matrix, reconstruct the exact transitions
    // The matrix shows the following transitions (reading from the image):
    
    // S0: -0.700, 0.525 to S1, 0.175 to S2
    markov_set_rate(chain, 0, 1, 0.525);
    markov_set_rate(chain, 0, 2, 0.175);
    
    // S1: 0.100 to S0, -0.275, 0 to S2, 0.175 to S3
    markov_set_rate(chain, 1, 0, 0.100);
    markov_set_rate(chain, 1, 3, 0.175);
    
    // S2: 0.100 to S0, 0 to S1, -0.800, 0.525 to S3, 0.175 to S4
    markov_set_rate(chain, 2, 0, 0.100);
    markov_set_rate(chain, 2, 3, 0.525);
    markov_set_rate(chain, 2, 4, 0.175);
    
    // S3: 0 to S0, 0.100 to S1, 0.100 to S2, -0.375, 0 to S4, 0 to S5, 0 to S6, 0.175 to S7
    markov_set_rate(chain, 3, 1, 0.100);
    markov_set_rate(chain, 3, 2, 0.100);
    markov_set_rate(chain, 3, 7, 0.175);
    
    // S4: 0 to S0-S2, 0.100 to S3, 0 to S4, -0.800, 0.175 to S5, 0 to S6, 0.525 to S7
    markov_set_rate(chain, 4, 2, 0.100);
    markov_set_rate(chain, 4, 5, 0.175);
    markov_set_rate(chain, 4, 7, 0.525);
    
    // S5: 0, 0, 0, 0, 0.100 to S4, -0.800, 0.175 to S6, 0, 0.525 to S8
    markov_set_rate(chain, 5, 4, 0.100);
    markov_set_rate(chain, 5, 6, 0.175);
    markov_set_rate(chain, 5, 8, 0.525);
    
    // S6: 0, 0, 0, 0, 0, 0.100 to S5, -0.625, 0, 0, 0.525 to S9
    markov_set_rate(chain, 6, 5, 0.100);
    markov_set_rate(chain, 6, 9, 0.525);
    
    // S7: 0, 0, 0, 0.100 to S3, 0.100 to S4, 0, 0, -0.375, 0.175 to S8, 0
    markov_set_rate(chain, 7, 3, 0.100);
    markov_set_rate(chain, 7, 4, 0.100);
    markov_set_rate(chain, 7, 8, 0.175);
    
    // S8: 0, 0, 0, 0, 0, 0.100 to S5, 0, 0.100 to S7, -0.375, 0.175 to S9
    markov_set_rate(chain, 8, 5, 0.100);
    markov_set_rate(chain, 8, 7, 0.100);
    markov_set_rate(chain, 8, 9, 0.175);
    
    // S9: 0, 0, 0, 0, 0, 0, 0.100 to S6, 0, 0.100 to S8, -0.200
    markov_set_rate(chain, 9, 6, 0.100);
    markov_set_rate(chain, 9, 8, 0.100);
    
    markov_finalize_matrix(chain);
}

void print_performance_metrics(MarkovChain *chain) {
    printf("\n=== PERFORMANCE METRICS FOR SYSTEM 1 ===\n");
    
    double p[10];
    for (int i = 0; i < 10; i++) {
        p[i] = markov_get_probability(chain, i);
    }
    
    // Utilization (ρ)
    // P1: probability that P1 is busy
    double rho_1 = p[1] + p[3] + p[6] + p[8] + p[9];
    // P2: probability that P2 is busy  
    double rho_2 = p[2] + p[3] + p[4] + p[5] + p[6] + p[7] + p[8] + p[9];
    double rho_total = (rho_1 + rho_2) / 2.0;
    
    printf("\nUtilization (ρ):\n");
    printf("  P1: %.6f\n", rho_1);
    printf("  P2: %.6f\n", rho_2);
    printf("  Total: %.6f\n", rho_total);
    
    // Loss probability (π)
    // P1: no queue, so π_1 = ρ_1 (always busy when job would arrive)
    double pi_1 = rho_1;
    // P2: queue full (states with 3 in queue)
    double pi_2 = p[7] + p[9];
    double pi_total = P1 * pi_1 + P2 * pi_2;
    
    printf("\nLoss Probability (π):\n");
    printf("  P1: %.6f\n", pi_1);
    printf("  P2: %.6f\n", pi_2);
    printf("  Total: %.6f\n", pi_total);
    
    // Queue length (l)
    // P1: no queue
    double l_1 = 0.0;
    // P2: expected number in queue
    double l_2 = 1.0*(p[4] + p[6]) + 2.0*(p[5] + p[8]) + 3.0*(p[7] + p[9]);
    double l_total = l_1 + l_2;
    
    printf("\nQueue Length (l):\n");
    printf("  P1: %.6f\n", l_1);
    printf("  P2: %.6f\n", l_2);
    printf("  Total: %.6f\n", l_total);
    
    // Number of jobs in system (m)
    double m_1 = l_1 + rho_1;
    double m_2 = l_2 + rho_2;
    double m_total = m_1 + m_2;
    
    printf("\nNumber of Jobs (m):\n");
    printf("  P1: %.6f\n", m_1);
    printf("  P2: %.6f\n", m_2);
    printf("  Total: %.6f\n", m_total);
    
    // Throughput (λ')
    double lambda_1_eff = (1.0 - pi_1) * LAMBDA_1;
    double lambda_2_eff = (1.0 - pi_2) * LAMBDA_2;
    double lambda_total_eff = lambda_1_eff + lambda_2_eff;
    
    printf("\nThroughput (λ'):\n");
    printf("  P1: %.6f\n", lambda_1_eff);
    printf("  P2: %.6f\n", lambda_2_eff);
    printf("  Total: %.6f\n", lambda_total_eff);
    
    // Idle probability (η)
    double eta_1 = 1.0 - rho_1;
    double eta_2 = 1.0 - rho_2;
    double eta_total = 1.0 - rho_total;
    
    printf("\nIdle Probability (η):\n");
    printf("  P1: %.6f\n", eta_1);
    printf("  P2: %.6f\n", eta_2);
    printf("  Total: %.6f\n", eta_total);
    
    // Waiting time (w)
    double w_1 = (lambda_1_eff > 0) ? l_1 / lambda_1_eff : 0.0;
    double w_2 = (lambda_2_eff > 0) ? l_2 / lambda_2_eff : 0.0;
    double w_total = (lambda_total_eff > 0) ? l_total / lambda_total_eff : 0.0;
    
    printf("\nWaiting Time (w):\n");
    printf("  P1: %.6f\n", w_1);
    printf("  P2: %.6f\n", w_2);
    printf("  Total: %.6f\n", w_total);
    
    // Residence time (u)
    double u_1 = w_1 + B;
    double u_2 = w_2 + B;
    double u_total = w_total + B;
    
    printf("\nResidence Time (u):\n");
    printf("  P1: %.6f\n", u_1);
    printf("  P2: %.6f\n", u_2);
    printf("  Total: %.6f\n", u_total);
    
    // Load (y)
    double y_1 = LAMBDA_1 * B;
    double y_2 = LAMBDA_2 * B;
    double y_total = y_1 + y_2;
    
    printf("\nLoad (y):\n");
    printf("  P1: %.2f\n", y_1);
    printf("  P2: %.2f\n", y_2);
    printf("  Total: %.2f\n", y_total);
}

void compare_with_expected(MarkovChain *chain) {
    printf("\n=== COMPARISON WITH PROVIDED VALUES ===\n");
    
    double expected[] = {0.007786, 0.040874, 0.013625, 0.071530, 0.023843, 
                        0.041726, 0.073020, 0.125178, 0.219061, 0.383357};
    
    printf("\nState | Calculated | Expected  | Difference\n");
    printf("------|------------|-----------|------------\n");
    
    double max_diff = 0.0;
    for (int i = 0; i < 10; i++) {
        double calc = markov_get_probability(chain, i);
        double diff = calc - expected[i];
        if (fabs(diff) > max_diff) {
            max_diff = fabs(diff);
        }
        printf("  S%-2d | %.6f | %.6f | %+.6f\n", i, calc, expected[i], diff);
    }
    
    printf("\nMaximum difference: %.6f\n", max_diff);
    
    if (max_diff < 0.001) {
        printf("✓ Results match within acceptable tolerance!\n");
    } else {
        printf("✗ Results differ significantly. Please verify transition matrix.\n");
    }
}

int main() {
    printf("=== MARKOVIAN QUEUING SYSTEM ANALYZER ===\n");
    printf("=== SYSTEM 1 VERIFICATION ===\n\n");
    
    printf("Parameters:\n");
    printf("  λ  = %.3f arrivals/sec\n", LAMBDA);
    printf("  b  = %.1f sec (mean service time)\n", B);
    printf("  μ  = %.3f completions/sec\n", MU);
    printf("  P1 = %.2f (probability to server 1)\n", P1);
    printf("  P2 = %.2f (probability to server 2)\n", P2);
    printf("  λ1 = %.3f arrivals/sec to P1\n", LAMBDA_1);
    printf("  λ2 = %.3f arrivals/sec to P2\n", LAMBDA_2);
    
    // Create Markov chain with 10 states
    MarkovChain *chain = markov_create(10);
    
    // Build transition matrix
    build_system1_matrix(chain);
    
    // Print matrix
    markov_print_matrix(chain);
    
    // Solve for steady-state
    printf("\nSolving for steady-state probabilities...\n");
    markov_solve_steady_state(chain);
    
    // Print results
    markov_print_steady_state(chain);
    
    // Compare with provided values
    compare_with_expected(chain);
    
    // Calculate and print performance metrics
    print_performance_metrics(chain);
    
    // Clean up
    markov_free(chain);
    
    return 0;
}
