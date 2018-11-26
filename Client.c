#include <stdio.h>          /* for printf() and fprintf() */
#include <stdlib.h>         /* for atoi() and exit() */
#ifdef _WIN32                   /* IF ON A WINDOWS PLATFORM YOU WILL HAVE TO CHECK THIS */
#include <winsock2.h>           /* IF ON A WINDOWS PLATFORM YOU WILL HAVE TO CHECK THIS */
#include <ws2tcpip.h>           /* IF ON A WINDOWS PLATFORM YOU WILL HAVE TO CHECK THIS */
#else
#include <arpa/inet.h>      /* for sockaddr_in and inet_ntoa() */
#include <sys/socket.h>     /* for socket(), bind(), and connect() */
#include <string.h>         /* for memset() */
#include <unistd.h>         /* for close() */
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "DieWithError.c"
#include "CaesarCipher.c"
#endif

#define PORT 5000 // server port
#define MAXLINE 1024 // buffer size
char* timeRequest; // message sent to the server to get current time (formatted GMT or Local)
void DieWithError(char *errorMessage);
char *CaesarCipher(int option, char str[]);
int userChoice; // user input choice

int Client(int operationType, int operationSubtype, int countryID){
    // handles the user's time request
    if(operationType == 1){
        if(operationSubtype == 1){
            int sockfd;
            char buffer[MAXLINE];
            timeRequest = "GMT"; // telling the server to return GMT format
            struct sockaddr_in servaddr;
            
            int n, len;
            // Creating socket file descriptor
            if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                printf("socket creation failed");
                exit(0);
            }
            
            memset(&servaddr, 0, sizeof(servaddr));
            
            // Filling server information
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(PORT);
            servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            // send time request to server
            sendto(sockfd, (const char*)timeRequest, strlen(timeRequest),
                   0, (const struct sockaddr*)&servaddr,
                   sizeof(servaddr));
            
            // receive server's response
            printf("GMT Time: ");
            n = recvfrom(sockfd, (char*)buffer, MAXLINE,
                         0, (struct sockaddr*)&servaddr,
                        (socklen_t *) &len);
            puts(buffer);
            close(sockfd);
            return 0;
        }
        
        else if(operationSubtype == 2){
            int sockfd;
            char buffer[MAXLINE];
            timeRequest = "Local"; // telling the server to return local format
            struct sockaddr_in servaddr;
            
            int n, len;
            // Creating socket file descriptor
            if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                printf("socket creation failed");
                exit(0);
            }
            
            memset(&servaddr, 0, sizeof(servaddr));
            
            // Filling server information
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(PORT);
            servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            // send hello message to server
            sendto(sockfd, (const char*)timeRequest, strlen(timeRequest), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
            
            // receive server's response
            printf("Local Time: ");
            n = recvfrom(sockfd, (char*)buffer, MAXLINE,
                         0, (struct sockaddr*)&servaddr,
                        (socklen_t *) &len);
            puts(buffer);
            close(sockfd);
            return 0;
        }
    }
    
    // handles the user's quote requests
    else if(operationType == 2){
        int sockfd;
        char buffer[MAXLINE];
        struct sockaddr_in servaddr;
        
        // Creating socket file descriptor
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("socket creation failed");
            exit(0);
        }
        
        memset(&servaddr, 0, sizeof(servaddr));
        
        // Filling server information
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        if (connect(sockfd, (struct sockaddr*)&servaddr,
                    sizeof(servaddr)) < 0) {
            printf("\n Error : Connect Failed \n");
        }
        
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "Quote");
        // Request quote of the day from the server
        write(sockfd, buffer, sizeof(buffer));
        printf("Quote: ");
        // Receive and decrypt the quote from the server
        read(sockfd, buffer, sizeof(buffer));
        char* quote = CaesarCipher(2, buffer);
        // Get quote line index
        char subbuff[3];
        memcpy(subbuff, &quote[0], 2);
        subbuff[2] = '\0';
        int line = atoi(subbuff);
        line = line - 1;
        // get a like or dislike response from the user to send to the server
        printf("Do you like the quote?\n");
        printf("1: Yes\n2: No\n");
        scanf("%d", &userChoice);
        
        // Send like/dislike back to server
        if(userChoice == 1){
            snprintf(buffer, sizeof(buffer), "%02dL", line);
            write(sockfd, buffer, sizeof(buffer));
            printf("Liked\n");
        }
        else if(userChoice == 2){
            snprintf(buffer, sizeof(buffer), "%02dD", line);
            write(sockfd, buffer, sizeof(buffer));
            printf("Disliked\n");
        }
        
        close(sockfd);
    }
    
    // handles the user's country info requests
    else if(operationType == 3){
        int sockfd, userChoice;
        char buffer[MAXLINE];
        char *msg = "CC"; // request code sent to server for country information
        char *request; // request variable containing the code for the specific type of info being returned
        struct sockaddr_in servaddr;
        
        // Creating socket file descriptor
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("socket creation failed");
            exit(0);
        }
        
        memset(&servaddr, 0, sizeof(servaddr));
        
        // Filling server information
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            printf("\n Error : Connect Failed \n");
        }
        
        // Send Country ID request to server
        write(sockfd, msg, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        puts(buffer);
        
        // Send Country ID to server
        snprintf(buffer, sizeof(buffer), "%d", countryID);
        write(sockfd, buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        puts(buffer);
        
        // ask the user what type of info they want from the server
        printf("1: Country Code\n2: Country Name\n3: Country Continent\n");
        scanf("%d", &userChoice);
        if(userChoice == 1){
            request = "K";
            write(sockfd, request, sizeof(buffer));
            printf("Country Code: ");
        }
        else if(userChoice == 2){
            request = "N";
            write(sockfd, request, sizeof(buffer));
            printf("Country Name: ");
        }
        else if(userChoice == 3){
            request = "C";
            write(sockfd, request, sizeof(buffer));
            printf("Country Continent: ");
        }
        
        // receive and decrypt returned country information
        read(sockfd, buffer, sizeof(buffer));
        CaesarCipher(2, buffer);
        close(sockfd);
    }
    return 0;
}
