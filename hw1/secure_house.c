#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum BLANK_CNT { INSERT_CNT = 2, TURN_CNT = 1, ENTER_CNT = 1, WHOS_CNT = 0, CHANGE_LOCKS_CNT = 1, LEAVE_CNT = 1 };

// void inputString(FILE *fp, size_t size, char *str) {
//   // The size is extended by the input with the value of the provisional
//   int ch;
//   size_t len = 0;
//   str = realloc(NULL, sizeof(*str) * size); // size is start size
//   if (!str)
//     return;
//   while (EOF != (ch = fgetc(fp)) && ch != '\n') {
//     str[len++] = ch;
//     if (len == size) {
//       str = realloc(str, sizeof(*str) * (size += 16));
//       if (!str)
//         return;
//     }
//   }
//   str[len++] = '\0';

//   str = realloc(str, sizeof(*str) * len);
// }

// void string_copy(char *to, const char *from) {
//   while ((*to++ = *from++) != '\0')
//     ;
// }

char *extract_first_word(char **from) {
  // Find the end of the first word
  char *endOfWord = NULL;
  char *firstWord = NULL;

  if (*from) {
    endOfWord = *from;
    // When *from is NULL, return NULL
    while (*endOfWord != '\0' && !isspace(*endOfWord))
      endOfWord++;
    // Calculate the length of the first word
    size_t firstWordLen = endOfWord - *from;

    // Allocate memory for the first word
    firstWord = (char *)malloc((firstWordLen + 1) * sizeof(char));
    if (firstWord == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }

    // Copy the first word to the new string
    strncpy(firstWord, *from, firstWordLen);
    firstWord[firstWordLen] = '\0';

    // Update the input string pointer to point to the remaining part after the first word
    if (*endOfWord == '\0')
      *from = NULL;
    else
      *from = endOfWord + 1;
    // *from = endOfWord;
  } else {
    firstWord = (char *)malloc(sizeof(char));
    firstWord[0] = '\0';
  }

  return firstWord;
}

void add_str(char ***array, const char *str, unsigned *size) {
  (*size)++;
  *array = realloc(*array, (*size) * sizeof(char *));
  (*array)[(*size) - 1] = malloc(strlen(str) + 1);
  strcpy((*array)[(*size) - 1], str);
}

void remove_user(char ***array, int index, unsigned *size) {
  free((*array)[index]);
  for (int i = index; i < (*size) - 1; i++) {
    (*array)[i] = (*array)[i + 1];
  }
  (*size)--;
  *array = realloc(*array, (*size) * sizeof(char *));
}

int get_whitespace(char **input) {
  char *current = *input;
  int cnt = 0;

  if (*input) {
    while (*current != '\0') {
      if (isspace(*current)) {
        if ((*(current + 1) != '\0' && !isspace(*(current + 1)))) {
          cnt++;
        } else
          return -1;
      }
      ++current;
    }
  } else
    cnt = -1;

  return cnt;
}

// NOTE: main function
int main(int argc, char *argv[]) {
  char *owner = NULL;
  char **keys = NULL;
  unsigned total_keys = 0;
  char **users_in = NULL;
  unsigned total_in = 0;
  char *inserted_key = NULL;
  int is_opened = 0;
  unsigned k = 0;

  // [x] Handle the owner and the first keys: ./secure_house <owner_name> <key_1> <key_2> ... <key_n>
  owner = (char *)malloc(strlen(argv[1]) + 1);
  // string_copy(owner, argv[1]);
  // [x] Need to check
  strcpy(owner, argv[1]);
  add_str(&keys, "FIREFIGHTER_SECRET_KEY", &total_keys);
  for (int i = 2; i < argc; i++)
    add_str(&keys, argv[i], &total_keys);

  // WARNING: ==================debugging==================
  // printf("Owner: %s\n", owner);
  // for (int i = 0; i < total_keys; i++)
  //   printf("Key: %s\n", keys[i]);
  // WARNING: ==================debugging==================

  int stop = 0; // Flag to indicate when to stop the loop
  int size = 10;
  char *input = NULL;
  char *originalInput = NULL;
  char *word = NULL;

  while (!stop) {
    // WARNING: ==================debugging==================
    // printf("====================================================\n");
    // if (users_in != NULL)
    //   for (int i = 0; i < total_in; i++)
    //     printf("%s, ", users_in[i]);
    // printf("\nnumber of keys: %d, ", total_keys);
    // if (keys != NULL)
    //   for (int i = 0; i < total_keys; i++)
    //     printf("%s, ", keys[i]);
    // printf("\n");
    // printf("Enter a string: ");
    // WARNING: ==================debugging==================

    // Allocate initial memory for the input string
    input = (char *)malloc(sizeof(char));
    if (input == NULL) {
      printf("Memory allocation failed. Exiting...\n");
      exit(EXIT_FAILURE);
    }

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
    originalInput = input;

    if (input[0] == '\0') {
      stop = 1; // Set the flag to stop the loop
    } else {
      // Process the input (if needed)
      word = extract_first_word(&input);

      if (strcmp(word, "INSERT") == 0) { // [x] INSERT KEY <user_name> <key>
        free(word);
        word = NULL;
        int cnt = get_whitespace(&input);
        if (cnt == INSERT_CNT) {
          word = extract_first_word(&input);
          if (strcmp(word, "KEY") == 0) {
            free(word);
            word = NULL;
            word = extract_first_word(&input);
            if (inserted_key != NULL) {
              free(inserted_key);
              inserted_key = NULL;
            }
            inserted_key = extract_first_word(&input);
            // NOTE: Reset the lock state
            is_opened = 0;
            printf("KEY %s INSERTED BY %s\n", inserted_key, word);
            if (word) {
              free(word);
              word = NULL;
            }
          } else {
            printf("ERROR\n");
          }
        } else
          printf("ERROR\n");
      } else if (strcmp(word, "TURN") == 0) { // [x] TURN KEY <user_name>
        int cnt = get_whitespace(&input);
        if (cnt == TURN_CNT) {
          free(word);
          word = NULL;
          word = extract_first_word(&input);
          if (strcmp(word, "KEY") == 0) {
            free(word);
            word = NULL;
            word = extract_first_word(&input);
            // [x] check if the inserted key is in the keys
            if (inserted_key == NULL)
              printf("FAILURE %s HAD NO KEY INSERTED\n", word);
            else {
              int is_found = 0;
              for (int i = 0; i < total_keys; i++) {
                if (strcmp(inserted_key, keys[i]) == 0) {
                  is_found = 1;
                  break;
                }
              }
              if (is_found == 1) {
                // NOTE: The only case for opening the door
                is_opened = 1;
                printf("SUCCESS %s TURNS KEY %s\n", word, inserted_key);
              } else
                printf("FAILURE %s HAD INVALID KEY %s INSERTED\n", word, inserted_key);
            }
            free(word);
            word = NULL;
          } else
            printf("ERROR\n");
        } else
          printf("ERROR\n");
      } else if (strcmp(word, "ENTER") == 0) { // [x] ENTER HOUSE <user_name>
        free(word);
        word = NULL;
        int cnt = get_whitespace(&input);
        if (cnt == ENTER_CNT) {
          word = extract_first_word(&input);
          if (strcmp(word, "HOUSE") == 0) {
            free(word);
            word = NULL;
            if (is_opened == 0) {
              printf("ACCESS DENIED\n");
            } else {
              word = extract_first_word(&input);

              // [x] Insert the user to users_in
              add_str(&users_in, word, &total_in);
              printf("ACCESS ALLOWED\n");
              // NOTE: Reset the lock state
              is_opened = 0;
              free(word);
              word = NULL;
            }
          } else
            printf("ERROR\n");
        } else
          printf("ERROR\n");
      } else if (strcmp(word, "WHO'S") == 0) { // [x] WHO'S INSIDE?
        free(word);
        word = NULL;
        int cnt = get_whitespace(&input);
        if (cnt == WHOS_CNT) {
          word = extract_first_word(&input);
          if (strcmp(word, "INSIDE?") == 0) {
            free(word);
            word = NULL;
            if (users_in == NULL || users_in[0] == NULL)
              printf("NOBODY HOME\n");
            else // [x] Maybe we need to use "total_in"
                 // [x] Format need to be changed
              for (int i = 0; i < total_in; i++) {
                if (i == total_in - 1)
                  printf("%s\n", users_in[i]);
                else
                  printf("%s, ", users_in[i]);
              }
          } else
            printf("ERROR\n");
        } else
          printf("ERROR\n");
      } else if (strcmp(word, "CHANGE") == 0) { // [x] CHANGE LOCKS <user_name> <key_1> <key_2> ... <key_n>
        free(word);
        word = NULL;
        word = extract_first_word(&input);
        if (strcmp(word, "LOCKS") == 0) {
          free(word);
          word = NULL;
          int last_idx = strlen(input) - 1;
          if (input[last_idx] == ' ')
            printf("ERROR\n");
          else {
            word = extract_first_word(&input);
            if (word[0] != '\0') {
              if (strcmp(word, owner) == 0) { // check if the user is equal to the owner
                int is_found = 0;
                // for (int i = 0; users_in[i] != NULL; i++) {
                for (int i = 0; i < total_in; i++) {
                  if (strcmp(owner, users_in[i]) == 0) { // check if the owner is in the house
                    is_found = 1;
                    break;
                  }
                }
                if (is_found) {
                  char *key = NULL;
                  int is_error = 0;
                  int total_keys_temp = 0;
                  char **keys_temp = NULL;
                  char **temp = NULL;
                  add_str(&keys_temp, "FIREFIGHTER_SECRET_KEY", &total_keys_temp);

                  // free(keys);
                  // keys = NULL;
                  // total_keys = 0;
                  // add_str(&keys, "FIREFIGHTER_SECRET_KEY", &total_keys);
                  // do-while -> while
                  while (input && *input) {
                    key = extract_first_word(&input);
                    if (!(*key)) {
                      free(key);
                      key = NULL;
                      is_error = 1;
                      break;
                    }
                    add_str(&keys_temp, key, &total_keys_temp);
                    free(key);
                    key = NULL;
                  }
                  if (is_error) {
                    for (int i = 0; i < total_keys_temp; i++)
                      free(keys_temp[i]);
                    free(keys_temp);
                    printf("ERROR\n");
                  } else {
                    temp = keys;
                    keys = keys_temp;
                    for (int i = 0; i < total_keys; i++)
                      free(temp[i]);
                    free(temp);
                    total_keys = total_keys_temp;
                    // NOTE: Reset the lock state
                    is_opened = 0;
                    printf("LOCK CHANGED\n");
                    // WARNING: ==================debugging==================
                    // for (int i = 0; i < total_keys; i++) {
                    //   printf("%s, ", keys[i]);
                    // }
                    // printf("\n");
                    // WARNING: ==================debugging==================
                  }
                } else
                  printf("LOCK CHANGE DENIED\n");
              } else
                printf("LOCK CHANGE DENIED\n");
            } else
              printf("ERROR\n");
            free(word);
            word = NULL;
          }
        } else
          printf("ERROR\n");

      } else if (strcmp(word, "LEAVE") == 0) { // [x] LEAVE HOUSE <user_name>
        free(word);
        word = NULL;
        int cnt = get_whitespace(&input);
        if (cnt == LEAVE_CNT) {
          word = extract_first_word(&input);
          if (strcmp(word, "HOUSE") == 0) {
            free(word);
            word = NULL;
            word = extract_first_word(&input);
            int u_idx = -1;
            for (int i = 0; i < total_in; i++) {
              if (strcmp(word, users_in[i]) == 0) {
                u_idx = i;
                break;
              }
            }
            if (u_idx != -1) {
              remove_user(&users_in, u_idx, &total_in);
              printf("%s LEFT\n", word);
            } else {
              printf("%s NOT HERE\n", word);
            }
            free(word);
            word = NULL;
          } else {
            printf("ERROR\n");
          }
        } else
          printf("ERROR\n");
      } else
        printf("ERROR\n");
    }

    // Free the dynamically allocated memory
    input = originalInput;
    free(input);
    input = NULL;
    originalInput = NULL;
  }

  // Free the existing variables
  if (owner != NULL) {
    free(owner);
    owner = NULL;
  }
  for (int i = 0; i < total_keys; i++) {
    free(keys[i]);
    keys[i] = NULL;
  }
  if (keys != NULL) {
    free(keys);
    keys = NULL;
  }
  for (int i = 0; i < total_in; i++) {
    free(users_in[i]);
    users_in[i] = NULL;
  }
  if (users_in != NULL) {
    free(users_in);
    users_in = NULL;
  }
  if (inserted_key != NULL) {
    free(inserted_key);
    inserted_key = NULL;
  }

  return 0;
}
