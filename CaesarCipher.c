#include <stdio.h>
#include <stdlib.h>

char *CaesarCipher(int option, char str[])
{
    int i;
    char *errorMsg = "\nError\n";
    switch(option)
    {
        case 1:
            //Encrypt the string
            for(i = 0; (i < 100 && str[i] != '\0'); i++)
            str[i] = str[i] + 3; //the key for encryption is 3 that is added to ASCII value
            printf("%s\n", str);
            return str;

        case 2:
            //Decrypt the string
            for(i = 0; (i < 100 && str[i] != '\0'); i++)
            str[i] = str[i] - 3; //the key for encryption is 3 that is subtracted to ASCII value
            printf("%s\n", str);
            return str;
        default:
            printf("\nError\n");
            return(errorMsg);
   }

}
