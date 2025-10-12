#include "mlp.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


double my_exp(double x) {
    if (x < 0) return 1.0 / my_exp(-x);

    double result = 1.0;
    double term = 1.0;

    for (int n = 1; n < 30; n++) {
        term *= x / n;
        result += term;
    }
    return result;
}


double sigmoid(double x) {
    if (x < 0) {
        double exp_x = my_exp(x);
        return exp_x / (1.0 + exp_x);
    }
    return 1.0 / (1.0 + my_exp(-x));
}
double dsigmoid(double y) {
    return y * (1.0 - y);
}

static void layer_init(Layer *layer, size_t inputs, size_t outputs) {
    layer->inputs = inputs;
    layer->outputs = outputs;
    layer->weights = malloc(inputs * outputs * sizeof(double));
    layer->biases = malloc(outputs * sizeof(double));
    layer->outputs_cache = malloc(outputs * sizeof(double));
    layer->inputs_cache = malloc(inputs * sizeof(double));
    layer->deltas = malloc(outputs * sizeof(double));

    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    for (size_t i = 0; i < inputs * outputs; ++i)
        layer->weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    for (size_t j = 0; j < outputs; ++j)
        layer->biases[j] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
}

static void layer_free(Layer *layer) {
    free(layer->weights);
    free(layer->biases);
    free(layer->outputs_cache);
    free(layer->inputs_cache);
    free(layer->deltas);
}

MLP *mlp_create(size_t *layers_sizes, size_t n_layers, double learning_rate) {
    MLP *mlp = malloc(sizeof(MLP));
    mlp->n_layers = n_layers - 1;
    mlp->learning_rate = learning_rate;
    mlp->layers = malloc(mlp->n_layers * sizeof(Layer));

    for (size_t i = 0; i < mlp->n_layers; ++i)
        layer_init(&mlp->layers[i], layers_sizes[i], layers_sizes[i + 1]);

    return mlp;
}

void mlp_free(MLP *mlp) {
    for (size_t i = 0; i < mlp->n_layers; ++i)
        layer_free(&mlp->layers[i]);
    free(mlp->layers);
    free(mlp);
}

void mlp_forward(MLP *mlp, const double *inputs) {
    const double *current_inputs = inputs;
    for (size_t i = 0; i < mlp->n_layers; ++i) {
        Layer *L = &mlp->layers[i];
        for (size_t j = 0; j < L->inputs; ++j)
            L->inputs_cache[j] = current_inputs[j];

        for (size_t o = 0; o < L->outputs; ++o) {
            double sum = L->biases[o];
            for (size_t k = 0; k < L->inputs; ++k)
                sum += L->weights[o * L->inputs + k] * current_inputs[k];
            L->outputs_cache[o] = sigmoid(sum);
        }
        current_inputs = L->outputs_cache;
    }
}

void mlp_train(MLP *mlp, const double *inputs, const double *targets, size_t epochs) {
    for (size_t e = 0; e < epochs; ++e) {
        mlp_forward(mlp, inputs);

        for (int i = mlp->n_layers - 1; i >= 0; --i) {
            Layer *L = &mlp->layers[i];

            if (i == (int)mlp->n_layers - 1) {
                for (size_t j = 0; j < L->outputs; ++j) {
                    double out = L->outputs_cache[j];
                    L->deltas[j] = (targets[j] - out) * dsigmoid(out);
                }
            } else {
                Layer *next = &mlp->layers[i + 1];
                for (size_t j = 0; j < L->outputs; ++j) {
                    double err = 0.0;
                    for (size_t k = 0; k < next->outputs; ++k)
                        err += next->deltas[k] * next->weights[k * next->inputs + j];
                    L->deltas[j] = err * dsigmoid(L->outputs_cache[j]);
                }
            }

            const double *prev_out = (i == 0) ? inputs : mlp->layers[i - 1].outputs_cache;
            for (size_t o = 0; o < L->outputs; ++o) {
                for (size_t k = 0; k < L->inputs; ++k) {
                    double grad = L->deltas[o] * prev_out[k];
                    L->weights[o * L->inputs + k] += mlp->learning_rate * grad;
                }
                L->biases[o] += mlp->learning_rate * L->deltas[o];
            }
        }
    }
}

typedef struct {
    double **weight_grads;
    double **bias_grads;
} BatchGradients;

static BatchGradients* create_batch_gradients(MLP *mlp) {
    BatchGradients *grads = malloc(sizeof(BatchGradients));
    grads->weight_grads = malloc(mlp->n_layers * sizeof(double*));
    grads->bias_grads = malloc(mlp->n_layers * sizeof(double*));

    for (size_t i = 0; i < mlp->n_layers; i++) {
        Layer *L = &mlp->layers[i];
        grads->weight_grads[i] = calloc(L->inputs * L->outputs, sizeof(double));
        grads->bias_grads[i] = calloc(L->outputs, sizeof(double));
    }

    return grads;
}

static void free_batch_gradients(BatchGradients *grads, MLP *mlp) {
    for (size_t i = 0; i < mlp->n_layers; i++) {
        free(grads->weight_grads[i]);
        free(grads->bias_grads[i]);
    }
    free(grads->weight_grads);
    free(grads->bias_grads);
    free(grads);
}

void mlp_train_batch(MLP *mlp, const double **inputs, const double **targets,
                    size_t batch_size, size_t epochs) {

    for (size_t epoch = 0; epoch < epochs; epoch++) {

        BatchGradients *batch_grads = create_batch_gradients(mlp);

        for (size_t b = 0; b < batch_size; b++) {
            mlp_forward(mlp, inputs[b]);

            for (int i = mlp->n_layers - 1; i >= 0; --i) {
                Layer *L = &mlp->layers[i];

                if (i == (int)mlp->n_layers - 1) {
                    for (size_t j = 0; j < L->outputs; ++j) {
                        double out = L->outputs_cache[j];
                        L->deltas[j] = (targets[b][j] - out) * dsigmoid(out);
                    }
                } else {
                    Layer *next = &mlp->layers[i + 1];
                    for (size_t j = 0; j < L->outputs; ++j) {
                        double err = 0.0;
                        for (size_t k = 0; k < next->outputs; ++k)
                            err += next->deltas[k] * next->weights[k * next->inputs + j];
                        L->deltas[j] = err * dsigmoid(L->outputs_cache[j]);
                    }
                }

                const double *prev_out = (i == 0) ? inputs[b] : mlp->layers[i - 1].outputs_cache;

                for (size_t o = 0; o < L->outputs; ++o) {
                    for (size_t k = 0; k < L->inputs; ++k) {
                        double grad = L->deltas[o] * prev_out[k];
                        batch_grads->weight_grads[i][o * L->inputs + k] += grad;
                    }
                    batch_grads->bias_grads[i][o] += L->deltas[o];
                }
            }
        }

        for (size_t i = 0; i < mlp->n_layers; i++) {
            Layer *L = &mlp->layers[i];

            for (size_t o = 0; o < L->outputs; ++o) {
                for (size_t k = 0; k < L->inputs; ++k) {
                    double avg_grad = batch_grads->weight_grads[i][o * L->inputs + k] / batch_size;
                    L->weights[o * L->inputs + k] += mlp->learning_rate * avg_grad;
                }
                double avg_bias_grad = batch_grads->bias_grads[i][o] / batch_size;
                L->biases[o] += mlp->learning_rate * avg_bias_grad;
            }
        }

        free_batch_gradients(batch_grads, mlp);

        if (epoch % 100 == 0) {
            double total_error = 0.0;
            for (size_t b = 0; b < batch_size; b++) {
                mlp_forward(mlp, inputs[b]);
                Layer *out_layer = &mlp->layers[mlp->n_layers - 1];
                for (size_t o = 0; o < out_layer->outputs; o++) {
                    double error = targets[b][o] - out_layer->outputs_cache[o];
                    total_error += error * error;
                }
            }
            printf("Epoch %zu, Average Error: %f\n", epoch, total_error / batch_size);
        }
    }
}

void mlp_predict(MLP *mlp, const double *inputs, double *outputs) {
    mlp_forward(mlp, inputs);
    Layer *out = &mlp->layers[mlp->n_layers - 1];
    for (size_t i = 0; i < out->outputs; ++i)
        outputs[i] = out->outputs_cache[i];
}