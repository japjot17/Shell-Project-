#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "util.c"

#define    MAX_SIZE_CMD    200
#define    MAX_SIZE_ARG    16


char cmd[MAX_SIZE_CMD] = {0};               // string holder for the command
char *argv[MAX_SIZE_ARG];            // an array for command and arguments
// global variable for the child process ID
char i;                                                // global for loop counter
char commands[200][200];
int countCommandsInFile = 0;
char *builtIn[] = {"print", "log", "theme", "exit"};
void get_cmd();                                // get command string from the user
void convert_cmd();                        // convert the command string to the required format by execvp()
void c_shell();                                // to start the shell
void executeCommand();
void exitShell();


int main(int argc, char *argvs[]) {


   // start the shell

   for (int p = 0; p < 200; p++) {
       for (int j = 0; j < 200; j++) {
           commands[p][j] = '\0';
       }
   }
   if (argc >= 2) {
       char *filename = argvs[1];
       FILE *fp = fopen(filename, "r");

       if (fp == NULL) {
           printf("Unable to read script file: %s", filename);
           return 1;
       }

       // read one character at a time and
       // display it to the output
       char ch;
       char line[200];

       for (int p = 0; p < 200; p++) {
           line[p] = '\0';
       }

       int p = 0;
       while ((ch = fgetc(fp)) != EOF) {
           line[p] = ch;
           p++;
       }


       // close the file
       fclose(fp);

       for (int p = 0; p < 200 - 5; p++) {
           if (line[p] == 'l') {
               if (line[p + 1] == 'o') {
                   if (line[p + 2] == 'g') {
                       if (line[p + 3] == '\0') {
                           countCommandsInFile++;
                           break;
                       }
                   }
               }
           }
           if (line[p] == '\n') {
               countCommandsInFile++;
           }
       }

       // printf("%d", countCommandsInFile);


       for (int p = 0; p < countCommandsInFile; p++) {
           for (int j = 0; j < 200; j++) {
               commands[p][j] = '\0';
           }
       }

       int index = 0;

       for (int p = 0; p < countCommandsInFile; p++) {
           for (int j = 0; j < 200; j++) {
               if (line[index] != '\n') {
                   commands[p][j] = line[index];
                   index++;
               } else if (line[index] == '\n') {
                   index++;
                   break;
               }
           }

       }

   }


   c_shell();


   return 0;
}

// Used these links for fork and exec help
// https://www.youtube.com/watch?v=zDjLADJGXFs&ab_channel=Alpha-BITs
// https://github.com/AlphaArslan/C-Simple-Linux-Shell-Wrapper/blob/master/code/shell.c
void c_shell() {
   while (1) {
       // get the command from user
       // clearArray(cmd,256);

       // If reading from a file in script mode
       if (commands[0][0] != '\0') {
           char local[200];

           for (int itr = 0; itr < countCommandsInFile; itr++) {
               clearArray(cmd, 200);
               clearArray(local, 200);
               for (int j = 0; j < 200; j++) {
                   if (commands[itr][j] == '\0') {
                       break;
                   }
                   local[j] = commands[itr][j];
                   cmd[j] = commands[itr][j];
               }
               if (cmd[0] == '$') {
                   size_t size = sizeof(cmd) / sizeof(cmd[0]);
                   EnvVar_t * envVar = linkEnvVar(cmd, size);
                   if(envVar != NULL) {
                       if (envVar->statusCode == INVALID_VARIABLE) {
                           linkCommand(cmd, 1);
                       } else {
                           linkCommand(cmd, 0);
                       }
                   }

               } else if (strcmp(cmd, "log") == 0) {
                   printListCommand(ctHead);
               } else if (strstr(cmd, "print")) {
                   int status = 0;
                   char *p;
                   p = strtok(cmd, " ");
                   p = strtok(NULL, " ");
                   while (p != NULL) {
                       //printf("DEBUG: %s\n", p);
                       if (p[0] == '$') {
                           char name[200] = {0};
                           parseName(p, name, 200);
                           EnvVar_t *tempHead = head;
                           char* found;
                           while (tempHead != NULL) {
                               int check = compareArraysName(name, tempHead->name, 200);
                               found = tempHead->value;
                               if (check == 0) {
                                   break;
                               }
                               tempHead = (EnvVar_t *) tempHead->next;
                           }
                           if (found != NULL) {
                               printf("%s ", found);
                           } else {
                               printf("Error: No Environment Variable $%s found.", name);
                               status = NO_ENVIRONMENT_VAR_FOUND;
                           }
                           //status = printEnvVars(p);
                       } else {
                           printf("%s ", p);
                       }
                       p = strtok(NULL, " ");
                   }
                   if(status == NO_ENVIRONMENT_VAR_FOUND) {
                       linkCommand(cmd, 1);
                   } else {
                       linkCommand(cmd, 0);

                   }
                   printf("\n");
               } else if(strstr(cmd, "theme")) {
                   char *p;
                   p = strtok(cmd, " ");
                   char *color = strtok(NULL, " ");
                   int status = changeTheme(color);
                   if(status == 0) {
                       linkCommand(cmd, 0);
                   } else {
                       linkCommand(cmd, 1);
                   }
               } else {
                   for (int itr = 0; itr < 4; itr++) {

                       if (strstr(cmd, builtIn[itr])) {
                           if (strcmp(cmd, "exit") == 0) {         // IF user enters exit
                               exitShell();
                           }
                       }
                   }


                   // bypass empty commands
                   if (!strcmp("", cmd)) continue;


                   executeCommand();
               }

           }
           exitShell();
       } else {
           // Interactive mode
           get_cmd();
           if (cmd[0] == '$') {
               size_t size = sizeof(cmd) / sizeof(cmd[0]);
               EnvVar_t * envVar = linkEnvVar(cmd, size);
               if(envVar != NULL) {
                   if (envVar->statusCode == INVALID_VARIABLE) {
                       linkCommand(cmd, 1);
                   } else {
                       linkCommand(cmd, 0);
                   }
               }

           } else if (strcmp(cmd, "log") == 0) {
               printListCommand(ctHead);
           } else if (strstr(cmd, "print")) {
               int status = 0;
               char *p;
               p = strtok(cmd, " ");
               p = strtok(NULL, " ");
               while (p != NULL) {
                   //printf("DEBUG: %s\n", p);
                   if (p[0] == '$') {
                       char name[200] = {0};
                       parseName(p, name, 200);
                       EnvVar_t *tempHead = head;
                       char* found;
                       while (tempHead != NULL) {
                           int check = compareArraysName(name, tempHead->name, 200);
                           found = tempHead->value;
                           if (check == 0) {
                               break;
                           }
                           tempHead = (EnvVar_t *) tempHead->next;
                       }
                       if (found != NULL) {
                           printf("%s ", found);
                       } else {
                           printf("Error: No Environment Variable $%s found.", name);
                           status = NO_ENVIRONMENT_VAR_FOUND;
                       }
                       //status = printEnvVars(p);
                   } else {
                       printf("%s ", p);
                   }
                   p = strtok(NULL, " ");
               }
               if(status == NO_ENVIRONMENT_VAR_FOUND) {
                   linkCommand(cmd, 1);
               } else {
                   linkCommand(cmd, 0);

               }
               printf("\n");
           } else if(strstr(cmd, "theme")) {
               char *p;
               p = strtok(cmd, " ");
               char *color = strtok(NULL, " ");
               int status = changeTheme(color);
               if(status == 0) {
                   linkCommand(cmd, 0);
               } else {
                   linkCommand(cmd, 1);
               }
           } else {
               for (int itr = 0; itr < 4; itr++) {

                   if (strstr(cmd, builtIn[itr])) {
                       if (strcmp(cmd, "exit") == 0) {         // IF user enters exit
                           exitShell();
                       }
                   }
               }


               // bypass empty commands
               if (!strcmp("", cmd)) continue;


               executeCommand();
           }
       }


   }
}

void get_cmd() {
   // get command from user
   printf("cshell$\t");
   fgets(cmd, MAX_SIZE_CMD, stdin);

   // remove trailing newline
   if ((strlen(cmd) > 0) && (cmd[strlen(cmd) - 1] == '\n'))
       cmd[strlen(cmd) - 1] = '\0';
   //printf("%s\n", cmd);
}

void convert_cmd() {
   // split string into argv
   char *ptr;
   i = 0;
   ptr = strtok(cmd, " ");
   while (ptr != NULL) {
       //printf("%s\n", ptr);
       argv[i] = ptr;
       i++;
       ptr = strtok(NULL, " ");
   }

   argv[i] = NULL;

   //printf("%d\n", i);
}

void executeCommand() {
   // fit the command into *argv[]
   convert_cmd();

   // fork and execute the command
   pid_t pid = fork();
   if (pid == -1) {
       printf("failed to create a child\n");
   } else if (pid == 0) {
       // execute a command
       //printf("%s", cmd);
       execute = execvp(argv[0], argv);
       int num = execute;
       exit(execute);
   } else {
      // printf("hello from parent\n");
       // wait for the command to finish if "&" is not present
       //printf("Parent: %d\n",execute);
       int childStatus;
       if (NULL == argv[i]) {
           waitpid(pid, &childStatus, 0);
       }
       if(childStatus != 0) {
           printf("Missing keyword or command, or permission problem\n");
           linkCommand(cmd, 1);
       } else {
           linkCommand(cmd, 0);
       }
   }

   if (NULL == argv[i]) {
       waitpid(pid, NULL, 0);
   }
}

void exitShell() {
   printf("Bye\n");
   printf("\033[0m");
   freeListEnvVar(head);
   freeListCommand(ctHead);
   exit(0);
}


