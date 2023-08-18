#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sodium.h> // For randombytes_buf function

#include "constants.h"

double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

double random_double(double min, double max)
{
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
