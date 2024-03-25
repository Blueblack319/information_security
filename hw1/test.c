#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *extract_first_word(char **from) {
  // Find the end of the first word
  char *endOfWord = *from;
  while (*endOfWord != '\0' && !isspace(*endOfWord))
    endOfWord++;

  // Calculate the length of the first word
  size_t firstWordLen = endOfWord - *from;

  // Allocate memory for the first word
  char *firstWord = (char *)malloc((firstWordLen + 1) * sizeof(char));
  if (firstWord == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  // Copy the first word to the new string
  strncpy(firstWord, *from, firstWordLen);
  firstWord[firstWordLen] = '\0';

  // Update the input string pointer to point to the remaining part after the first word
  *from = endOfWord + 1;

  return firstWord;
}

// NOTE: main function
int main(int argc, char *argv[]) {
  char *owner;
  char **keys = NULL;
  unsigned total_keys = 0;
  char *inserted_key = NULL;
  int is_opened = 0;
  char **users_in = NULL;
  unsigned total_in = 0;

  int stop = 0; // Flag to indicate when to stop the loop
  int size = 10;

  while (1) {
    printf("Enter a string: ");
    char *input = NULL;

    // Allocate initial memory for the input string
    input = (char *)malloc(sizeof(char));
    if (input == NULL) {
      printf("Memory allocation failed. Exiting...\n");
      exit(EXIT_FAILURE);
    }
    char *originalInput = strdup(input);

    // NOTE: Version1
    // Read input character by character and resize the buffer as needed
    char c;
    int i = 0;
    while (EOF != (c = fgetc(stdin)) && c != '\n') {
      input[i++] = c;
      input = (char *)realloc(input, (i + 1) * sizeof(char));
      if (input == NULL) {
        printf("Memory reallocation failed. Exiting...\n");
        exit(EXIT_FAILURE);
      }
    }
    input[i] = '\0'; // Null-terminate the input string

    char *word;
    int count = 10;
    do {
      word = extract_first_word(&input);
      printf("word: %s\n", word);
      printf("input: %s\n", input);
      count--;
      if (count == 0)
        break;
    } while (*input != '\0');
  }
}