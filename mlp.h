#ifndef MLP_H
#define MLP_H

#include <stddef.h>

typedef struct {
    size_t inputs;
    size_t outputs;
    double *weights;
    double *biases;
    double *outputs_cache;
    double *inputs_cache;
    double *deltas;
} Layer;

typedef struct {
    size_t n_layers;
    Layer *layers;
    double learning_rate;
} MLP;

MLP *mlp_create(size_t *layers_sizes, size_t n_layers, double learning_rate);
void mlp_free(MLP *mlp);

void mlp_forward(MLP *mlp, const double *inputs);
void mlp_train(MLP *mlp, const double *inputs, const double *targets, size_t epochs);

void mlp_predict(MLP *mlp, const double *inputs, double *outputs);

double sigmoid(double x);
double dsigmoid(double y);

#endif
