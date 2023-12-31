#ifndef CONSTANT_H
#define CONSTANT_H

#define num_inputs 2          // N1 = 2
#define num_neurons_layer2 40 // N2 = 40
#define num_neurons_layer3 20 // N3 = 20
#define num_outputs 2         // N4 = 2
#define initial_range 0.2

#define Learning_rate 0.005 // 0 - 1, 0.01 - 0.0001
#define epochs 100000

#define MAX_ROWS 48120
#define MAX_COLS 4
#define train_split 0.001 // 0.001 percent of data will be used for train
#define num_train (int)(MAX_ROWS * train_split)
#define num_val (int)(MAX_ROWS * (1-train_split))

#endif // CONSTANT_H
