#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAXLINES 43

char *random_quote(char *userFile){
    FILE *file;
    char input[255];
    char *lines[MAXLINES];
    char *randomLine;
    srand(time(NULL));
    
    file = fopen(userFile, "r");
    if(file == NULL)
    {
        printf("Can't open file");
    }
    
    else{
        int i = 0;
        while (fgets(input, sizeof(input), file) != NULL){
            lines[i] = strdup(input);
            i++;
        }
    }
    
    fclose(file);
    
    printf("\n\n");
    int num = (rand() % (42 - 0 + 1)) + 0;
    snprintf(input, sizeof(input), "%02d: %s", num, lines[num]);
    randomLine = input;
    
    return randomLine;
}
