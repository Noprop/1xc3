#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "constants.h"
#include "mymodel.h"

int main() {
  // initialize the 2D array to store the data
  double data[MAX_ROWS][MAX_COLS];
  // pointer to data, the 'size' of the pointer is the size of one row.
  double (*data_ptr)[MAX_COLS] = data;
  readFile(data_ptr);

  double X_train[num_train][num_inputs];
  double Y_train[num_train][num_outputs];
  double X_val[num_val][num_inputs];
  double Y_val[num_val][num_outputs];

  for (int row = 0; row < num_train; row++) {
    X_train[row][0] = data[row][0];
    X_train[row][1] = data[row][1];
    Y_train[row][0] = data[row][2];
    Y_train[row][1] = data[row][3];
  }

  for (int row = num_train; row < MAX_ROWS; row++) {
    X_val[row - num_train][0] = data[row][0];
    X_val[row - num_train][1] = data[row][1];
    Y_val[row - num_train][0] = data[row][2];
    Y_val[row - num_train][1] = data[row][3];
  }

  double W2[num_neurons_layer2][num_inputs];
  double b2[num_neurons_layer2][1];

  double W3[num_neurons_layer3][num_neurons_layer2];
  double b3[num_neurons_layer3][1];

  double W4[num_outputs][num_neurons_layer3];
  double b4[num_outputs][1];

  initWeightsAndBiases(W2, b2, W3, b3, W4, b4);

  // the final output of each layer, every column is for a set of inputs
  double a2[num_neurons_layer2][num_train];
  double a3[num_neurons_layer3][num_train];
  double a4[num_outputs][num_train];

  for (int ep = 0; ep < epochs; ep++) {
    forwardPass(X_train, Y_train,
                W2, W3, W4,
                b2, b3, b4,
                a2, a3, a4);

    backwardPass(X_train, Y_train,
                 W2, W3, W4,
                 b2, b3, b4,
                 a2, a3, a4);

    // QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
    // ###################################################### Evaluation of accuracies starts
    if (ep%100 == 0) {
      evaluation(X_train, Y_train,
                 X_val, Y_val,
                 W2, W3, W4,
                 b2, b3, b4,
                 a2, a3, ep);
    }
  }

  return 0;
}
