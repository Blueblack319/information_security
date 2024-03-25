#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // for getlogin()

#define MAX_LINE_LENGTH 1000
#define LOG_FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP)
#define TEST_FILE_MODE \
  (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
const char *levels[] = {"TOP_SECRET", "SECRET", "CONFIDENTIAL", "UNCLASSIFIED"};
const char *file_names[] = {"top_secret.data", "secret.data",
                            "confidential.data", "unclassified.data"};

int checkFileExists(const char *filename) {
  struct stat buffer;
  return stat(filename, &buffer) == 0 ? 1 : 0;
}

mode_t readUmask() {
  mode_t um = umask(0);
  umask(um);
  return um;
}

int main(int argc, char *argv[]) {
  // [x] Get inputs
  char *command;
  char *file_name;

  command = argv[1];
  file_name = argv[2];

  // [x] Read the mac.policy file with the root privilege
  // Get the current user's name
  // char *current_user = getenv("USER");
  // if (current_user == NULL) {
  //   perror("Error getting current user");
  //   return 1;
  // }
  char *current_user;
  uid_t uid = getuid();
  gid_t gid = getuid();
  // Retrieve the username associated with the UID
  struct passwd *pw = getpwuid(uid);
  if (pw == NULL) {
    perror("Error getting username");
    return 1;
  }
  current_user = pw->pw_name;

  // Open the file for reading
  FILE *file = fopen("mac.policy", "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // [x] Find and compare the user is specified in mac.policy file
  // Buffer to hold each line read from the file
  char line[MAX_LINE_LENGTH];
  int is_found = 0;
  char *level;
  // Loop to read each line from the file
  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    // Search for "david" in the current line
    if (strstr(line, current_user) != NULL) {
      // [x] if then, get the security clearance level of the user
      level = strchr(line, ':');
      if (level != NULL) {
        level++;
      }
      size_t level_size = strlen(level);
      if (level[level_size - 1] == '\n') {
        level[level_size - 1] = '\0';
      }

      //==========================debug===========================
      printf("Level:%s\n", level);
      //==========================debug===========================

      is_found = 1;
      break;
    }
  }
  // [x] else, print ACCESS DENIED & exit
  if (!is_found) {
    printf("ACCESS DENIED(in specifying)\n");
    return 0;
  }

  // Close the file
  fclose(file);

  // [x] Validate read-down or write-up with the level and command
  // [x] Find the index of levels which value is matched to level and
  // [x] Do the same to the file_name variable
  int idx_level = 0;
  int idx_file_name = 0;
  for (; idx_level < 4; idx_level++) {
    if (strncmp(level, levels[idx_level], strlen(level)) == 0) break;
  }
  for (; idx_file_name < 4; idx_file_name++) {
    if (strncmp(file_name, file_names[idx_file_name], strlen(file_name)) == 0)
      break;
  }

  //==========================debug===========================
  printf("user's level: %d\n", idx_level);
  printf("target level: %d\n", idx_file_name);
  //==========================debug===========================

  // [ ] TEST: Is there any other command except for read and write? any other
  // error case?
  if (strcmp(command, "read") == 0) {
    // NOTE: Perform the command if following read-down policy
    if (idx_level > idx_file_name) {
      // [x] else, print ACCESS DENIED
      printf("ACCESS DENIED(in read)\n");
    } else {
      // Reads and prints the whole content of the document file
      char ch;
      FILE *document_file = fopen(file_name, "r");
      // Check if file opened successfully
      if (document_file == NULL) {
        printf("Error: %s", strerror(errno));
        return 1;
      }
      // Read character by character until EOF is reached
      while ((ch = fgetc(document_file)) != EOF) {
        // Print the character to standard output
        printf("%c", ch);
      }
      printf("\n");

      // Close the file
      fclose(document_file);
    }
  } else {
    // NOTE: Perform the command if following write-up policy
    if (idx_level < idx_file_name)
      printf("ACCESS DENIED(in write)\n");
    else {
      char *data = argv[3];
      FILE *document_file = fopen(file_name, "a");
      // Check if file opened successfully
      if (document_file == NULL) {
        printf("Error: %s", strerror(errno));
        return 1;
      }
      // Append the data into document file
      fprintf(document_file, "%s\n", data);
      // Close the file
      fclose(document_file);
    }
  }
  // NOTE: Dropping the root privilege
  // [x] We no longer need root priviledge so drop the privilege
  // [x] test: the owners of data files will be changed if the part is
  // removed? Drop privileges back to the current user's
  if (setegid(gid) != 0 || seteuid(uid) != 0) {
    perror("Error dropping privileges");
    return 1;
  }

  // NOTE: logging
  // Open the file for reading
  strcat(current_user, ".log");
  FILE *log_file;
  if (!checkFileExists(current_user)) {
    // [x] IF the log file does not exist, create it
    // [x] TEST: without umask(0)
    umask(0);
    creat(current_user, LOG_FILE_MODE);
    log_file = fopen(current_user, "w");
    if (log_file == NULL) {
      printf("Error opening a file: %s\n", current_user);
      return 1;
    }
  } else {
    // [x] If the file already exist, append a string
    log_file = fopen(current_user, "a");
  }
  fprintf(log_file, "%s %s\n", command, file_name);
  // Close the file
  fclose(log_file);
  return 0;
}