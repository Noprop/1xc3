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

	int i;
	for (i = 0; i < len * 2; i++) {
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