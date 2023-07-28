#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "constants.h"
#include "readFile_randWord.h"
#include "functions.h"

int main() {

	// ##############################################################################
	// ############################# Dictionary #####################################
	// ##############################################################################
	char **words = NULL;
	int numWords = 0;
	char *filename = "words_alpha.txt";

	// Read words from the file and store them in the dynamically allocated array
	readWords(filename, &words, &numWords);

	// Pick a random word with at least minLength characters from the array
	char *randomWord = getRandomWordWithMinLength(words, numWords, MIN_RAND_WORD_LENGTH);

	char choice;
	printf("Do you want to play the automatic version? (y/n): ");
	scanf(" %c", &choice);
	if (choice == 'y') {
		printf("\n\nAutomatic Version.\n");
		printf("\nThe server has generated a random word. You have %d attempts to guess the word.\n", MAX_ATTEMPTS);

		// Call the automatic version of the word guessing game
		int numSuggestion = 50;
		bool gameResult_auto = playWordGuessingGameAutomatic(randomWord, words, numWords, numSuggestion);

		if (gameResult_auto) {
			printf("Congratulations! You guessed the word correctly: %s\n", randomWord);
		} else {
			printf("Out of attempts. The word was: %s\n", randomWord);
		}
	} else {
		printf("\nThe server has generated a random word. You have %d attempts to guess the word.\n", MAX_ATTEMPTS);
		bool gameResult = playWordGuessingGame(randomWord);

		if (gameResult) {
			printf("Congratulations! You guessed the word correctly: %s\n", randomWord);
		}
		else {
			printf("Out of attempts. The word was: %s\n", randomWord);
		}
	}

	// Free allocated memory for words
	freeWords(words, numWords);

	return 0;
}
