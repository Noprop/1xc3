#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "constants.h"

bool playWordGuessingGame(char *word)
{
	char guess;
	int attempt = 1;
	size_t charsFound = 0;
	size_t len = strlen(word);								// size_t is used for better size representation
	char *userExperience = malloc(len*2 + 1); // a char is 1 byte, so no need to multiply in the malloc
	if (userExperience == NULL)								// and userExp is the string printed to the console
		return 0;

	for (size_t i = 0; i < len*2; i++) {
		// setup our user string
		userExperience[i] = i % 2 ? ' ' : '-';
	}

	printf("%s\n", userExperience);

	while (attempt <= MAX_ATTEMPTS) {
		char *search;
		char *wordIter = word;  // always init to the start of the word

		// using a do loop here so we don't repeat our user input
		// code in the instance that they keep guessing correctly
		do {
			printf("Attempt %i: ", attempt);
			scanf(" %c", &guess); // read in the guess

			while ((wordIter = strchr(wordIter, guess)) != NULL) {
				charsFound++;
				int idx = wordIter - word;     // find the relative idx
				userExperience[idx*2] = guess; // update our ui
				wordIter += 1;                 // look at the rest of the word
			}

			printf("%s\n", userExperience);
			wordIter = word;       // reset the pointer
			if (charsFound == len) // user found the word!
				return true;
		} while ((search = strchr(word, guess)) != NULL); // keep guessing...


		if (attempt == MAX_ATTEMPTS)
			break;
		attempt++;
		printf("Incorrect guess. Try again.\n"); // not at max attempts yet.
	}

	return false;
}

bool playWordGuessingGameAutomatic (const char *randomWord, char **words, int numWords, int numSuggestion) {
	char guess;
	int attempt = 1;
	size_t charsFound = 0;
	size_t len = strlen(randomWord);					// size_t is used for better size representation
	char *userExperience = malloc(len*2 + 1); // a char is 1 byte, so no need to multiply in the malloc
	if (userExperience == NULL)								// and userExp is the string printed to the console
		return 0;

	for (size_t i = 0; i < len*2; i++) {
		// setup our user string
		userExperience[i] = i % 2 ? ' ' : '-';
	}

	printf("%s\n", userExperience);

	while (attempt <= MAX_ATTEMPTS) {
		char *search;
		const char *wordIter = randomWord; // always init to the start of the word
                                       // also change to const b/c of the parameter

		// using a do loop here so we don't repeat our user input
		// code in the instance that they keep guessing correctly
		do {
			printf("Attempt %i: ", attempt);
			scanf(" %c", &guess); // read in the guess

			while ((wordIter = strchr(wordIter, guess)) != NULL) {
				charsFound++;
				int idx = wordIter - randomWord; // find the relative idx
				userExperience[idx*2] = guess;   // update our ui
				wordIter += 1;                   // look at the rest of the word
			}

			printf("%s\n", userExperience);
			wordIter = randomWord; // reset the pointer
			if (charsFound == len) // user found the word!
				return true;
		} while ((search = strchr(randomWord, guess)) != NULL); // keep guessing...


		if (attempt == (MAX_ATTEMPTS-1)) {
			// create a static 2d array of the max amount of suggestions possible
			char potentialWords[numSuggestion][len+1]; // +1 to include the null terminator
			int wordCount = 0;

			// iterate while not at the end of the list and have space left
			for (int i = 0; i < numWords && wordCount < numSuggestion; i++) {
				if (strlen(words[i]) == len) { // recall len is from our randomWord
					bool flag = 1;

					// no need to compare if they haven't found a single character
					if (charsFound != 0) {
						for (size_t j = 0; j < len; j++) {
							if (userExperience[j*2] == '-') continue; // no need to check
							if (words[i][j] != randomWord[j]) {
								// if it's not what what we're looking for disable the flag
								flag = 0;
								break;
							}
						}
					}

					if (flag) {
						// save to word bank
						strcpy(potentialWords[wordCount], words[i]);
						wordCount++;
					}
				}
			}
			printf("The number of possible words = %i\n", wordCount);
			printf("Here is up to the first %i possible words\n\n", numSuggestion);

			// print out the list of words
			for (int i = 0; i < wordCount; i++) {
				printf("%s,\n", potentialWords[i]);
			}
			printf("\n");
		}

		if (attempt == MAX_ATTEMPTS) break;
		attempt++;
		printf("Incorrect guess. Try again.\n"); // not at max attempts yet.
	}

	return false;
}