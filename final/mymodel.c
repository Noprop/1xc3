#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sodium.h> // For randombytes_buf function

#include "constants.h"

double sigmoid(double x) {
  return 1.0 / (1.0 + exp(-x));
}

double randomDouble(double min, double max) {
  unsigned char buffer[sizeof(uint64_t)]; // Buffer to hold random bytes
  uint64_t random_value;

  // Initialize the sodium library
  if (sodium_init() < 0) {
    // Initialization failed
    // Handle the error here (e.g., print an error message and exit)
  }

  // Generate random bytes
  randombytes_buf(buffer, sizeof(uint64_t));

  // Convert the random bytes to a 64-bit unsigned integer value
  memcpy(&random_value, buffer, sizeof(uint64_t));

  // Scale the 64-bit random integer to a double value in the range [0, 1.0]
  double scale = random_value / ((double)UINT64_MAX);

  // Scale the value to the desired range [min, max]
  return min + scale * (max - min);
}

void readFile(double (*data_ptr)[MAX_COLS]) {
  // Open the file in read mode
  FILE *file = fopen("data.txt", "r");
  if (file == NULL) {
    printf("Error opening the file.\n");
    exit(EXIT_FAILURE);
  }

  int row = 0;
  char line[100];

  // Read each line of the file
  while (fgets(line, sizeof(line), file)) {
    // Tokenize the line and store the values in the array
    sscanf(line, "%lf %lf %lf %lf", &data_ptr[row][0], &data_ptr[row][1], &data_ptr[row][2], &data_ptr[row][3]);

    row++;
    // Break the loop if the array is full to prevent overflow
    if (row >= MAX_ROWS) {
      printf("End of the file.\n");
      break;
    }
  }

  // Close the file
  fclose(file);
}

void forwardPass(double X_train[][num_inputs], double Y_train[][num_outputs],
                 double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                 double b2[][1], double b3[][1], double b4[][1],
                 double a2[][num_train], double a3[][num_train], double a4[][num_train]
) {  
  for (int i = 0; i < num_neurons_layer2; i++) {
    for (int j = 0; j < num_train; j++) {
      double sum = 0;
      for (size_t k = 0; k < num_inputs; k++) {
        sum += W2[i][k] * X_train[j][k];
      }

      a2[i][j] = tanh(sum + b2[i][0]); // the activation function here is tanh()
    }
  }

  for (int i = 0; i < num_neurons_layer3; i++) {
    for (int j = 0; j < num_train; j++) {
      double sum = 0;
      for (size_t k = 0; k < num_neurons_layer2; k++) {
        sum += W3[i][k] * a2[k][j];
      }

      a3[i][j] = tanh(sum + b3[i][0]); // the activation function here is tanh()
    }
  }

  for (int i = 0; i < num_outputs; i++) {
    for (int j = 0; j < num_train; j++) {
      double sum = 0;
      for (size_t k = 0; k < num_neurons_layer3; k++) {
        sum += W4[i][k] * a3[k][j];
      }

      a4[i][j] = sigmoid(sum + b4[i][0]);
    }
  }
}

void backwardPass(double X_train[][num_inputs], double Y_train[][num_outputs],
                  double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                  double b2[][1], double b3[][1], double b4[][1],
                  double a2[][num_train], double a3[][num_train], double a4[][num_train]
) {
  double (*PL1)[num_train] = malloc(num_neurons_layer2 * sizeof(double[num_train]));
  double (*PL2)[num_train] = malloc(num_neurons_layer3 * sizeof(double[num_train]));
  double (*PL3)[num_train] = malloc(num_outputs * sizeof(double[num_train]));
  double (*a3_squared_complement)[num_train] = malloc(num_neurons_layer3 * sizeof(double[num_train]));
  double (*W4_PL3)[num_train] = malloc(num_neurons_layer3 * sizeof(double[num_train]));
  double (*a2_squared_complement)[num_train] = malloc(num_neurons_layer2 * sizeof(double[num_train]));
  double (*W3_PL2)[num_train] = malloc(num_neurons_layer2 * sizeof(double[num_train]));
  if (!PL1 || !PL2 || !PL3 || !a3_squared_complement ||
      !W4_PL3 || !a2_squared_complement || !W3_PL2) {
    printf("malloc failed!\n");
    exit(EXIT_FAILURE);
  }

  // Calculate PL2 = (a4 - Y_train).*(1-a4).*a4 if the activation function is sigmoid() in layer output
  for (int i = 0; i < num_outputs; i++) {
    for (int j = 0; j < num_train; j++) {
      PL3[i][j] = (a4[i][j] - Y_train[j][i]) * (1 - a4[i][j]) * a4[i][j];
    }
  }

  // Calculate PL2 = (1 - H3.^2) .* (W4' * PE3) if the activation function is tanh() in layer 3
  // Calculate (1 - a3.^2) and store the result in a3_squared_complement
  for (int i = 0; i < num_neurons_layer3; i++) {
    for (int j = 0; j < num_train; j++) {
      a3_squared_complement[i][j] = 1 - a3[i][j] * a3[i][j];
    }
  }

  // Calculate W4' * PL3 and store the result in W4_PL3
  for (int i = 0; i < num_neurons_layer3; i++) {
    for (int j = 0; j < num_train; j++) {
      double sum = 0.0;
      for (int k = 0; k < num_outputs; k++) {
        sum += W4[k][i] * PL3[k][j];
      }
      W4_PL3[i][j] = sum;
    }
  }

  // Calculate PE1 = (1 - H2.^2) .* (W3' * PE2) if the activation function is tanh() in layer 2
  for (int i = 0; i < num_neurons_layer3; i++) {
    for (int j = 0; j < num_train; j++) {
      PL2[i][j] = a3_squared_complement[i][j] * W4_PL3[i][j];
    }
  }

  // Calculate  PE1 = (1 - H2.^2) .* (W3' * PE2) the activation function is tanh()
  // Calculate (1 - a2.^2) and store the result in a2_squared_complement
  for (int i = 0; i < num_neurons_layer2; i++) {
    for (int j = 0; j < num_train; j++) {
      a2_squared_complement[i][j] = 1 - a2[i][j] * a2[i][j];
    }
  }

  // Calculate W3' * PL2 and store the result in W3_PL2
  for (int i = 0; i < num_neurons_layer2; i++) {
    for (int j = 0; j < num_train; j++) {
      double sum = 0.0;
      for (int k = 0; k < num_neurons_layer3; k++) {
        sum += W3[k][i] * PL2[k][j];
      }
      W3_PL2[i][j] = sum;
    }
  }

  // Calculate PL1 = (1 - a2.^2) .* (W3' * PL2) and store the result in PL1
  for (int i = 0; i < num_neurons_layer2; i++) {
    for (int j = 0; j < num_train; j++) {
      PL1[i][j] = a2_squared_complement[i][j] * W3_PL2[i][j];
    }
  }

  // Update weights and biases using learning_rate
  for (int i = 0; i < num_neurons_layer2; i++) {
    for (int j = 0; j < num_inputs; j++) {
      double sum = 0.0;
      for (int k = 0; k < num_train; k++) {
        sum += PL1[i][k] * X_train[k][j];
      }
      W2[i][j] -= Learning_rate * sum;
    }
  }

  for (int i = 0; i < num_neurons_layer3; i++) {
    for (int j = 0; j < num_neurons_layer2; j++) {
      double sum = 0.0;
      for (int k = 0; k < num_train; k++) {
        sum += PL2[i][k] * a2[j][k];
      }
      W3[i][j] -= Learning_rate * sum;
    }
  }

  for (int i = 0; i < num_outputs; i++) {
    for (int j = 0; j < num_neurons_layer3; j++) {
      double sum = 0.0;
      for (int k = 0; k < num_train; k++) {
        sum += PL3[i][k] * a3[j][k];
      }
      W4[i][j] -= Learning_rate * sum;
    }
  }

  // Update biases b2, b3, and b4
  for (int i = 0; i < num_neurons_layer2; i++) {
    double sum = 0.0;
    for (int j = 0; j < num_train; j++) {
      sum += PL1[i][j];
    }
    b2[i][0] -= Learning_rate * sum;
  }

  for (int i = 0; i < num_neurons_layer3; i++) {
    double sum = 0.0;
    for (int j = 0; j < num_train; j++) {
      sum += PL2[i][j];
    }
    b3[i][0] -= Learning_rate * sum;
  }

  for (int i = 0; i < num_outputs; i++) {
    double sum = 0.0;
    for (int j = 0; j < num_train; j++) {
      sum += PL3[i][j];
    }
    b4[i][0] -= Learning_rate * sum;
  }

  free(PL1);
  free(PL2);
  free(PL3);
  free(a3_squared_complement);
  free(W4_PL3);
  free(a2_squared_complement);
  free(W3_PL2);
}

void evaluation(double X_train[][num_inputs], double Y_train[][num_outputs],
                double X_val[][num_inputs], double Y_val[][num_outputs],
                double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                double b2[][1], double b3[][1], double b4[][1],
                double a2[][num_train], double a3[][num_train], int ep
) {
  double a4_eval_train[num_outputs][num_train];
  forwardPass(X_train, Y_train,
              W2, W3, W4,
              b2, b3, b4,
              a2, a3, a4_eval_train);

  // check predictions on training data
  int correct_predictions = 0;
  for (int i = 0; i < num_train; i++) {
    int all_correct = 1; // Assume all outputs are correct for this example
    for (int j = 0; j < num_outputs; j++) {
      if ((a4_eval_train[j][i] >= 0.5 && Y_train[i][j] == 0) || (a4_eval_train[j][i] < 0.5 && Y_train[i][j] == 1)) {
        all_correct = 0; // Found an incorrect prediction for this example
        break;
      }
    }
    correct_predictions += all_correct;
  }

  double accuracy_train = (double)correct_predictions / num_train * 100.0;

  double sum_squared_diff = 0.0;
  for (int i = 0; i < num_train; i++) {
    for (int j = 0; j < num_outputs; j++) {
      double diff = Y_train[i][j] - a4_eval_train[j][i]; // Transpose a4_eval_train by switching indices
      sum_squared_diff += diff * diff;
    }
  }

  // Calculate the cost and divide by num_train
  double cost_train = sum_squared_diff / num_train;

  // QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
  /// Explain here why I needed to dynamically allocate the memory

  // answer: you'll run out of memory on the stack. During testing, I actually encountered
  // this exact problem in the backwardPass function. In turn, I changed it to dynamic memory allocation
  // onto the heap, using malloc, and no longer ran into this problem.
  
  // What did I learn? How to debug. I was running into a segmentation fault in that specific function
  // and so I had to pull out the debugger and find out that after declaring a 2D array of a 20x4000 array
  // I'd run into a EXC_BAD_ACCESS after trying to modify [0][0] of the array and
  // figured ah, we've run out of space on the stack.

  double(*a2_val)[num_val] = malloc(num_neurons_layer2 * sizeof(double[num_val]));
  double(*a3_val)[num_val] = malloc(num_neurons_layer3 * sizeof(double[num_val]));
  double(*a4_val)[num_val] = malloc(num_outputs * sizeof(double[num_val]));

  forwardPass(X_val, Y_val,
              W2, W3, W4,
              b2, b3, b4,
              a2_val, a3_val, a4_val);

  // check predictions on real data
  correct_predictions = 0;
  for (int i = 0; i < num_val; i++) {
    int all_correct = 1; // Assume all outputs are correct for this example
    for (int j = 0; j < num_outputs; j++) {
      if ((a4_val[j][i] >= 0.5 && Y_val[i][j] == 0) || (a4_val[j][i] < 0.5 && Y_val[i][j] == 1)) {
        all_correct = 0; // Found an incorrect prediction for this example
        break;
      }
    }
    correct_predictions += all_correct;
  }

  double accuracy_val = (double)correct_predictions / num_val * 100.0;

  sum_squared_diff = 0.0;
  for (int i = 0; i < num_val; i++) {
    for (int j = 0; j < num_outputs; j++) {
      double diff = Y_val[i][j] - a4_val[j][i]; // Transpose a4_eval_train by switching indices
      sum_squared_diff += diff * diff;
    }
  }

  free(a2_val);
  free(a3_val);
  free(a4_val);

  // Calculate the cost and divide by num_train
  double cost_val = sum_squared_diff / num_val;

  printf("epoch %d:\n", ep);
  printf("Train Cost      %lf,    Accuracy: %.2f%%\n", cost_train, accuracy_train);
  printf("Validation Cost %lf,    Accuracy: %.2f%%\n\n", cost_val, accuracy_val);
}

void initWeightsAndBiases(double W2[][num_inputs], double b2[][1],
  												double W3[][num_neurons_layer2], double b3[][1],
													double W4[][num_neurons_layer3], double b4[][1]) {
	// Initialize W2 and b2 arrays with random values between -a and +a
  for (int i = 0; i < num_neurons_layer2; i++) {
    for (int j = 0; j < num_inputs; j++) {
      W2[i][j] = randomDouble(-initial_range, initial_range);
    }
    b2[i][0] = randomDouble(-initial_range, initial_range);
  }

  // Initialize W3 and b3 arrays with random values between -a and +a
  for (int i = 0; i < num_neurons_layer3; i++) {
    for (int j = 0; j < num_neurons_layer2; j++) {
      W3[i][j] = randomDouble(-initial_range, initial_range);
    }
    b3[i][0] = randomDouble(-initial_range, initial_range);
  }

  // Initialize W4 and b4 arrays with random values between -a and +a
  for (int i = 0; i < num_outputs; i++) {
    for (int j = 0; j < num_neurons_layer3; j++) {
      W4[i][j] = randomDouble(-initial_range, initial_range);
    }
    b4[i][0] = randomDouble(-initial_range, initial_range);
  }
}