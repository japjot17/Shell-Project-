// C Program to design a shell in Linux
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

int execute = 0;

#define    INVALID_VARIABLE    1
#define    NO_ENVIRONMENT_VAR_FOUND 2

// Environment variable struct
struct EnvVar {
   char name[200];
   char value[200];
   struct Envar *next;
   int statusCode;
};

typedef struct EnvVar EnvVar_t;

EnvVar_t *head = NULL;

void printListEnvVar(EnvVar_t *top) {
   EnvVar_t *temp = top;

   while (temp != NULL) {
       char *tmpName = temp->name;
       char *tmpValue = temp->value;
       int statusVal = top->statusCode;
       printf("%s", tmpName);
       printf("%s", tmpValue);
       printf("%d", statusVal);
       temp = (EnvVar_t *) temp->next;
       printf("\n");
   }
   printf("\n");
}

// https://www.youtube.com/watch?v=VOpjAHCee7c&ab_channel=JacobSorber
EnvVar_t *initEnvVar(const char *nameOfVar, const char *valueOfVar, int returnV) {
   EnvVar_t *result = malloc(sizeof(EnvVar_t));
   if (nameOfVar != NULL && valueOfVar != NULL) {
       for (int i = 0; i < 100; i++) {
           result->name[i] = nameOfVar[i];
       }
       for (int i = 0; i < 100; i++) {
           result->value[i] = valueOfVar[i];
       }
   }
   result->statusCode = returnV;
   result->next = NULL;
   return result;
}

// https://www.youtube.com/watch?v=VOpjAHCee7c&ab_channel=JacobSorber
void insertAtHeadEnvVar(EnvVar_t **top, EnvVar_t *nodeToInsert) {
   if (nodeToInsert != NULL) {
       nodeToInsert->next = (struct Envar *) *top;
       *top = nodeToInsert;
   }
}

int compareArrays(const char arr1[], const char arr2[]) {
   for (int i = 0; i < 100; i++) {
       if (arr1[i] != arr2[i]) {
           return 1;
       }
   }
   return 0;
}

int compareArraysName(const char arr1[], const char arr2[], int size) {
   for (int i = 0; i < size; i++) {
       char temp1 = arr1[i];
       char temp2 = arr2[i];
       if (arr2[i] == '\0') {
           break;
       }
       if (arr1[i] != arr2[i]) {
           return 1;
       }
   }
   return 0;
}

// https://www.youtube.com/watch?v=VOpjAHCee7c&t=816s&ab_channel=JacobSorber
EnvVar_t *findNodeEnvVar(EnvVar_t *top, char nameOfVar[]) {
   EnvVar_t *temp = top;
   while (temp != NULL) {
       if (compareArrays(nameOfVar, temp->name) == 0) {
           return temp;
       }
       temp = (EnvVar_t *) temp->next;
   }
   return NULL;
}

// https://www.delftstack.com/howto/c/free-linked-list-c/
void freeListEnvVar(EnvVar_t *headNode) {
   EnvVar_t *currentNode;
   while (headNode != NULL) {
       currentNode = headNode;
       headNode = (EnvVar_t *) headNode->next;
       free(currentNode);
   }
}

void clearArray(char arr[], int size) {
   for (int i = 0; i < size; i++) {
       arr[i] = 0;
   }
}

void errorEnvVar() {
   printf("Variable value expected\n");
}

void notVarFound() {
   printf("Error: No Environment Variable $b found.\n");
}


// Function takes in the input and parses different parts of createNodeEnvVar  given
EnvVar_t *createNodeEnvVar(const char *arr, EnvVar_t *top, int size) {
   int returnVal = 0;
   int checkIfEqualSignExists = 0;


   for (int i = 0; i < size; i++) {
       if (arr[i] == '=') {
           if (arr[i + 1] == ' ') {
               errorEnvVar();
               return initEnvVar(NULL, NULL, INVALID_VARIABLE);
           }
       }
       if (arr[i] == ' ') {
           if (arr[i + 1] == '=') {
               errorEnvVar();
               return initEnvVar(NULL, NULL, INVALID_VARIABLE);
           }
       }

   }
   for (int i = 0; i < size; i++) {
       if (arr[i] == '=') {
           checkIfEqualSignExists = 1;
       }
   }
   if (checkIfEqualSignExists == 0) {
       return initEnvVar(NULL, NULL, INVALID_VARIABLE);
   }

   char varName[100];
   clearArray(varName, sizeof(varName) / sizeof(char));
   char varValue[100];
   clearArray(varValue, sizeof(varValue) / sizeof(char));
   int counter = 0;
   for (int i = 0; i < size; i++) {
       if (arr[i] == '$') {
           for (int j = i + 1; arr[j] != '='; j++) {
               varName[j - 1] = arr[j];
               counter++;
           }
       }

       if (arr[i] == '=') {
           for (int j = 0; j < 100; j++) {
               if (arr[j + i + 1] == 0) {
                   break;
               }
               varValue[j] = arr[j + i + 1];
           }
       }
   }

   EnvVar_t *duplicateName = findNodeEnvVar(top, varName);
   if (duplicateName != NULL) {
       for (int i = 0; i < 100; i++) {
           duplicateName->value[i] = varValue[i];
       }
       return NULL;
   }

   if (varName[0] == '\0' || varValue[0] == '\0' || varValue[0] == ' ' || varName[0] == ' ') {
       return initEnvVar(NULL, NULL, INVALID_VARIABLE);
   }


   return initEnvVar(varName, varValue, returnVal);
}

EnvVar_t *linkEnvVar(char cmd[], int size) {
   size = size - 1;
   EnvVar_t *environVar = NULL;
   environVar = createNodeEnvVar(cmd, head, size);
   if (environVar != NULL) {
       insertAtHeadEnvVar(&head, environVar);
   }
   return environVar;
}

void parseName(const char cmd[], char name[], int size) {
   size = size - 1;
   int count = 0;
   for (int i = 0; i < size; i++) {
       if (cmd[i] == '$') {
           for (int j = i + 1; j < size; j++) {
               name[count] = cmd[j];
               count++;
           }
       }
   }
}

int printEnvVars(char* arrVar) {
   char name[200] = {0};
   parseName(arrVar, name, 200);
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
     printf("%s ", "found");
   } else {
       printf("Error: No Environment Variable $%s found.", name);
       return NO_ENVIRONMENT_VAR_FOUND;
   }
   return 0;
}

struct Command {
   char name[100];
   char time[24];
   int returnValue;
   struct Command *next;
};

typedef struct Command Command_t;

Command_t *ctHead = NULL;

void printListCommand(Command_t *top) {
   Command_t *temp = top;
   int count = 0;
   while (temp != NULL) {                                           // Get size of list
       temp = (Command_t *) temp->next;
       count++;
   }
   char *name[count];
   char *time[count];
   int retVals[count];
   temp = top;
   for (int i = 0; temp != NULL; temp = (Command_t *) temp->next) { // Insert values in 2 arrays
       name[i] = temp->name;
       time[i] = temp->time;
       retVals[i] = temp->returnValue;
       i++;
   }
   for (int i = count - 1; i >= 0; i--) {                            // Print array values from last index to 0
       for (int j = 0; j < 24; j++) {
           printf("%c", time[i][j]);
       }
       printf("\n%s ", name[i]);
       printf("%d\n", retVals[i]);
   }

}

Command_t *createNewNodeCommand(const char *nameOfCommand, const char *timeOfCommand, int returnVal) {
   Command_t *result = malloc(sizeof(Command_t));
   for (int i = 0; i < 100; i++) {
       result->name[i] = nameOfCommand[i];
   }

   for (int i = 0; i < 24; i++) {
       result->time[i] = timeOfCommand[i];
   }
   result->returnValue = returnVal;
   result->next = NULL;
   return result;
}

Command_t *insertAtHeadCommand(Command_t **top, Command_t *nodeToInsert) {
   nodeToInsert->next = (struct Command *) *top;
   *top = nodeToInsert;
   return nodeToInsert;
}

Command_t *findNodeCommand(Command_t *top, const char *nameOfCommand) {
   Command_t *temp = top;
   while (temp != NULL) {
       if (temp->name == nameOfCommand) {
           return temp;
       }
       temp = (Command_t *) temp->next;
   }
   return NULL;
}

void freeListCommand(Command_t *headNode) {
   Command_t *currentNode;
   while (headNode != NULL) {
       currentNode = headNode;
       headNode = (Command_t *) headNode->next;
       free(currentNode);
   }
}


char *intToMonth(int index) {
   char *allMonths[] = {"Jan", "Feb", "Mar", "Apr",
                        "May", "Jun", "Jul", "Aug",
                        "Sept", "Oct", "Nov", "Dec"};
   return allMonths[index];
}

char *intToDay(int index) {
   char *allDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
   return allDays[index];
}

// https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
void getDate(char *arr) {
   struct tm *tm;
   time_t rawtime;
   time(&rawtime);
   tm = localtime(&rawtime);
//    printf("%s ",intToDay(tm->tm_wday));
//    printf("%s ",intToMonth(tm->tm_mon));
//    printf("%02d %02d:%02d:%02d %d \n", tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_year + 1900);

   char monthDay[3];
   sprintf(monthDay, "%02d", tm->tm_mday);

   char hour[3];
   sprintf(hour, "%d", tm->tm_hour);

   char min[3];
   sprintf(min, "%02d", tm->tm_min);


   char sec[3];
   sprintf(sec, "%02d", tm->tm_sec);

   char year[5];
   sprintf(year, "%d", tm->tm_year + 1900);

   arr[0] = intToDay(tm->tm_wday)[0];
   arr[1] = intToDay(tm->tm_wday)[1];
   arr[2] = intToDay(tm->tm_wday)[2];
   arr[3] = ' ';
   arr[4] = intToMonth(tm->tm_mon)[0];
   arr[5] = intToMonth(tm->tm_mon)[1];
   arr[6] = intToMonth(tm->tm_mon)[2];
   arr[7] = ' ';
   arr[8] = monthDay[0];
   arr[9] = monthDay[1];
   arr[10] = ' ';
   arr[11] = hour[0];
   arr[12] = hour[1];
   arr[13] = ':';
   arr[14] = min[0];
   arr[15] = min[1];
   arr[16] = ':';
   arr[17] = sec[0];
   arr[18] = sec[1];
   arr[19] = ' ';
   arr[20] = year[0];
   arr[21] = year[1];
   arr[22] = year[2];
   arr[23] = year[3];
}

void linkCommand(char cmd[], int returnVal) {
   // for some reason we're passing 1 here.
   char dateArr[24];
   char *result = NULL;
   if (strstr(cmd, "print")) {
       result = "print";
   } else {
       result = cmd;
   }
   Command_t *commandNode = NULL;
   getDate(dateArr);
   commandNode = createNewNodeCommand(result, dateArr, returnVal);
   insertAtHeadCommand(&ctHead, commandNode);
}

int changeTheme(char *color) {

   if (color == NULL) {
       printf("unsupported theme: %s\n", color);
       printf("\033[0m");
       return 1;
   } else {
       for (int i = 0; i < strlen(color); i++) {
           color[i] = tolower((unsigned char) color[i]);
       }
       //checking for colors
       if (strcmp(color, "red") == 0) {
           printf("\033[0;31m");
       } else if (strcmp(color, "blue") == 0) {
           printf("\033[0;34m");
       } else if (strcmp(color, "green") == 0) {
           printf("\033[0;32m");
       } else {
           printf("unsupported theme: %s\n", color);
           printf("\033[0m");
           return 1;
       }
   }
   return 0;

}

