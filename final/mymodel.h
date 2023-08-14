#include "constants.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

double sigmoid(double x);

double randomDouble(double min, double max);

void readFile(double (*data_ptr)[MAX_COLS]);

void forwardPass(double X_train[][num_inputs],
                 double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                 double b2[][1], double b3[][1], double b4[][1],
                 double a2[][num_train], double a3[][num_train], double a4[][num_train]
);

void backwardPass(double X_train[][num_inputs], double Y_train[][num_outputs],
                  double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                  double b2[][1], double b3[][1], double b4[][1],
                  double a2[][num_train], double a3[][num_train], double a4[][num_train]
);

void evaluation(double X_train[][num_inputs], double Y_train[][num_outputs],
                double X_val[][num_inputs], double Y_val[][num_outputs],
                double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                double b2[][1], double b3[][1], double b4[][1],
                double a2[][num_train], double a3[][num_train], int ep
);

void initWeightsAndBiases(double W2[][num_inputs], double b2[][1],
                          double W3[][num_neurons_layer2], double b3[][1],
                          double W4[][num_neurons_layer3], double b4[][1]);

#endif