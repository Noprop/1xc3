#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "constants.h"
#include "mymodel.h"

void ForwardPass(int num_train, double X_train[][num_inputs],
                 double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                 double b2[][1], double b3[][1], double b4[][1],
                 double a2[][num_train], double a3[][num_train], double a4[][num_train]);

void BackwardPass(int num_train, double X_train[][num_inputs], double Y_train[][num_outputs],
                 double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                 double b2[][1], double b3[][1], double b4[][1],
                 double a2[][num_train], double a3[][num_train], double a4[][num_train]);

int main()
{
    // Open the file in read mode
    // initialize the 2D array to store the data
    double data[MAX_ROWS][MAX_COLS];
    // pointer to data, the 'size' of the pointer is the size of one row.
    double (*data_ptr)[MAX_COLS] = data;
    readFile(data_ptr);

    int num_train = MAX_ROWS *train_split;
    int num_val = MAX_ROWS *(1-train_split);

    double X_train[num_train][num_inputs];
    double Y_train[num_train][num_outputs];
    double X_val[num_val][num_inputs];
    double Y_val[num_val][num_outputs];

    for (int row = 0; row < num_train; row++)
    {
        X_train[row][0] = data[row][0];
        X_train[row][1] = data[row][1];
        Y_train[row][0] = data[row][2];
        Y_train[row][1] = data[row][3];
    }

    for (int row = num_train; row < MAX_ROWS; row++)
    {
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

    // Initialize W2 and b2 arrays with random values between -a and +a
    for (int i = 0; i < num_neurons_layer2; i++)
    {
        for (int j = 0; j < num_inputs; j++)
        {
            W2[i][j] = random_double(-initial_range, initial_range);
        }
        b2[i][0] = random_double(-initial_range, initial_range);
    }

    // Initialize W3 and b3 arrays with random values between -a and +a
    for (int i = 0; i < num_neurons_layer3; i++)
    {
        for (int j = 0; j < num_neurons_layer2; j++)
        {
            W3[i][j] = random_double(-initial_range, initial_range);
        }
        b3[i][0] = random_double(-initial_range, initial_range);
    }

    // Initialize W4 and b4 arrays with random values between -a and +a
    for (int i = 0; i < num_outputs; i++)
    {
        for (int j = 0; j < num_neurons_layer3; j++)
        {
            W4[i][j] = random_double(-initial_range, initial_range);
        }
        b4[i][0] = random_double(-initial_range, initial_range);
    }

    // the final output of each layer, every column is for a set of inputs
    double a2[num_neurons_layer2][num_train];
    double a3[num_neurons_layer3][num_train];
    double a4[num_outputs][num_train];

    for (int ep = 0; ep <= epochs; ep++)
    {
        ForwardPass(num_train, X_train,
                    W2, W3, W4,
                    b2, b3, b4,
                    a2, a3, a4);

        BackwardPass(num_train, X_train, Y_train,
                    W2, W3, W4,
                    b2, b3, b4,
                    a2, a3, a4);

        // QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
        // ###################################################### Evaluation of accuracies starts

        if (ep%10000==0)
        {
        double a4_eval_train[num_outputs][num_train];
        ForwardPass(num_train, X_train,
                    W2, W3, W4,
                    b2, b3, b4,
                    a2, a3, a4_eval_train);

        int correct_predictions = 0;
        for (int i = 0; i < num_train; i++)
        {
            int all_correct = 1; // Assume all outputs are correct for this example
            for (int j = 0; j < num_outputs; j++)
            {
                if ((a4_eval_train[j][i] >= 0.5 && Y_train[i][j] == 0) || (a4_eval_train[j][i] < 0.5 && Y_train[i][j] == 1))
                {
                    all_correct = 0; // Found an incorrect prediction for this example
                    break;
                }
            }
            correct_predictions += all_correct;
        }

        double accuracy_train = (double)correct_predictions / num_train * 100.0;

        double sum_squared_diff = 0.0;
        for (int i = 0; i < num_train; i++)
        {
            for (int j = 0; j < num_outputs; j++)
            {
                double diff = Y_train[i][j] - a4_eval_train[j][i]; // Transpose a4_eval_train by switching indices
                sum_squared_diff += diff * diff;
            }
        }

        // Calculate the cost and divide by num_train
        double cost_train = sum_squared_diff / num_train;

        

        // QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
        // Explain here why I needed to dynamically allocate the memory

        // answer: you'll run out of memory on the stack.
        // During testing (because the git repo files originally set training split to 0.1) I actually
        // encountered this exact problem in the backwardPass function. In turn, I changed it to dynamic
        // memory allocation onto the heap, using malloc, and no longer ran into this problem.
        
        // What did I learn? How to debug. I was running into a segmentation fault in that specific function
        // and so I had to pull out the debugger and find out that after declaring a 2D array of a 20x4000 array
        // I'd run into a EXC_BAD_ACCESS after trying to modify [0][0] of the array and
        // figured ah, we've run out of space on the stack.
        double(*a2_val)[num_val] = malloc(num_neurons_layer2 * sizeof(double[num_val]));
        double(*a3_val)[num_val] = malloc(num_neurons_layer3 * sizeof(double[num_val]));
        double(*a4_val)[num_val] = malloc(num_outputs * sizeof(double[num_val]));

        ForwardPass(num_val, X_val,
                    W2, W3, W4,
                    b2, b3, b4,
                    a2_val, a3_val, a4_val);

        correct_predictions = 0;
        for (int i = 0; i < num_val; i++)
        {
            int all_correct = 1; // Assume all outputs are correct for this example
            for (int j = 0; j < num_outputs; j++)
            {
                if ((a4_val[j][i] >= 0.5 && Y_val[i][j] == 0) || (a4_val[j][i] < 0.5 && Y_val[i][j] == 1))
                {
                    all_correct = 0; // Found an incorrect prediction for this example
                    break;
                }
            }
            correct_predictions += all_correct;
        }



        double accuracy_val = (double)correct_predictions / num_val * 100.0;


        sum_squared_diff = 0.0;
        for (int i = 0; i < num_val; i++)
        {
            for (int j = 0; j < num_outputs; j++)
            {
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
        printf("Train Cost      %lf,    Accuracy: %.2f%%\n",cost_train, accuracy_train);
        printf("Validation Cost %lf,    Accuracy: %.2f%%\n\n",cost_val, accuracy_val);


        }
    }
}


void ForwardPass(int num_train, double X_train[][num_inputs],
                 double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                 double b2[][1], double b3[][1], double b4[][1],
                 double a2[][num_train], double a3[][num_train], double a4[][num_train])
{
    for (int i = 0; i < num_neurons_layer2; i++)
    {
        for (int j = 0; j < num_train; j++)
        {
            double sum = 0;
            for (size_t k = 0; k < num_inputs; k++)
            {
                sum += W2[i][k] * X_train[j][k];
            }

            a2[i][j] = tanh(sum + b2[i][0]); // the activation function here is tanh()
        }
    }

    for (int i = 0; i < num_neurons_layer3; i++)
    {
        for (int j = 0; j < num_train; j++)
        {
            double sum = 0;
            for (size_t k = 0; k < num_neurons_layer2; k++)
            {
                sum += W3[i][k] * a2[k][j];
            }

            a3[i][j] = tanh(sum + b3[i][0]); // the activation function here is tanh()
        }
    }

    for (int i = 0; i < num_outputs; i++)
    {
        for (int j = 0; j < num_train; j++)
        {
            double sum = 0;
            for (size_t k = 0; k < num_neurons_layer3; k++)
            {
                sum += W4[i][k] * a3[k][j];
            }

            a4[i][j] = sigmoid(sum + b4[i][0]);
        }
    }
}

void BackwardPass(int num_train, double X_train[][num_inputs], double Y_train[][num_outputs],
                 double W2[][num_inputs], double W3[][num_neurons_layer2], double W4[][num_neurons_layer3],
                 double b2[][1], double b3[][1], double b4[][1],
                 double a2[][num_train], double a3[][num_train], double a4[][num_train])
{
  double PL1[num_neurons_layer2][num_train];
  double PL2[num_neurons_layer3][num_train];
  double PL3[num_outputs][num_train];

  // Calculate PL2 = (a4 - Y_train).*(1-a4).*a4 if the activation function is sigmoid() in layer output
  for (int i = 0; i < num_outputs; i++)
  {
      for (int j = 0; j < num_train; j++)
      {
          PL3[i][j] = (a4[i][j] - Y_train[j][i]) * (1 - a4[i][j]) * a4[i][j];
      }
  }

  // Calculate PL2 = (1 - H3.^2) .* (W4' * PE3) if the activation function is tanh() in layer 3
  // Calculate (1 - a3.^2) and store the result in a3_squared_complement
  double a3_squared_complement[num_neurons_layer3][num_train];
  for (int i = 0; i < num_neurons_layer3; i++)
  {
      for (int j = 0; j < num_train; j++)
      {
          a3_squared_complement[i][j] = 1 - a3[i][j] * a3[i][j];
      }
  }

  // Calculate W4' * PL3 and store the result in W4_PL3
  double W4_PL3[num_neurons_layer3][num_train];
  for (int i = 0; i < num_neurons_layer3; i++)
  {
      for (int j = 0; j < num_train; j++)
      {
          double sum = 0.0;
          for (int k = 0; k < num_outputs; k++)
          {
              sum += W4[k][i] * PL3[k][j];
          }
          W4_PL3[i][j] = sum;
      }
  }

  // Calculate PE1 = (1 - H2.^2) .* (W3' * PE2) if the activation function is tanh() in layer 2
  for (int i = 0; i < num_neurons_layer3; i++)
  {
      for (int j = 0; j < num_train; j++)
      {
          PL2[i][j] = a3_squared_complement[i][j] * W4_PL3[i][j];
      }
  }

  // Calculate  PE1 = (1 - H2.^2) .* (W3' * PE2) the activation function is tanh()
  // Calculate (1 - a2.^2) and store the result in a2_squared_complement
  double a2_squared_complement[num_neurons_layer2][num_train];
  for (int i = 0; i < num_neurons_layer2; i++)
  {
      for (int j = 0; j < num_train; j++)
      {
          a2_squared_complement[i][j] = 1 - a2[i][j] * a2[i][j];
      }
  }

  // Calculate W3' * PL2 and store the result in W3_PL2
  double W3_PL2[num_neurons_layer2][num_train];
  for (int i = 0; i < num_neurons_layer2; i++)
  {
      for (int j = 0; j < num_train; j++)
      {
          double sum = 0.0;
          for (int k = 0; k < num_neurons_layer3; k++)
          {
              sum += W3[k][i] * PL2[k][j];
          }
          W3_PL2[i][j] = sum;
      }
  }

  // Calculate PL1 = (1 - a2.^2) .* (W3' * PL2) and store the result in PL1
  for (int i = 0; i < num_neurons_layer2; i++)
  {
      for (int j = 0; j < num_train; j++)
      {
          PL1[i][j] = a2_squared_complement[i][j] * W3_PL2[i][j];
      }
  }

  // Update weights and biases using learning_rate
  for (int i = 0; i < num_neurons_layer2; i++)
  {
      for (int j = 0; j < num_inputs; j++)
      {
          double sum = 0.0;
          for (int k = 0; k < num_train; k++)
          {
              sum += PL1[i][k] * X_train[k][j];
          }
          W2[i][j] -= Learning_rate * sum;
      }
  }

  for (int i = 0; i < num_neurons_layer3; i++)
  {
      for (int j = 0; j < num_neurons_layer2; j++)
      {
          double sum = 0.0;
          for (int k = 0; k < num_train; k++)
          {
              sum += PL2[i][k] * a2[j][k];
          }
          W3[i][j] -= Learning_rate * sum;
      }
  }

  for (int i = 0; i < num_outputs; i++)
  {
      for (int j = 0; j < num_neurons_layer3; j++)
      {
          double sum = 0.0;
          for (int k = 0; k < num_train; k++)
          {
              sum += PL3[i][k] * a3[j][k];
          }
          W4[i][j] -= Learning_rate * sum;
      }
  }

  // Update biases b2, b3, and b4
  for (int i = 0; i < num_neurons_layer2; i++)
  {
      double sum = 0.0;
      for (int j = 0; j < num_train; j++)
      {
          sum += PL1[i][j];
      }
      b2[i][0] -= Learning_rate * sum;
  }

  for (int i = 0; i < num_neurons_layer3; i++)
  {
      double sum = 0.0;
      for (int j = 0; j < num_train; j++)
      {
          sum += PL2[i][j];
      }
      b3[i][0] -= Learning_rate * sum;
  }

  for (int i = 0; i < num_outputs; i++)
  {
      double sum = 0.0;
      for (int j = 0; j < num_train; j++)
      {
          sum += PL3[i][j];
      }
      b4[i][0] -= Learning_rate * sum;
  }
}

