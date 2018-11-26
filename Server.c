#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define PORT 5000
#define MAXLINE 1024
int max(int x, int y){
    if(x > y){
        return x;
    }
    else{
        return y;
    }
}

void DieWithError(char *errorMessage);
char *CaesarCipher(int option, char str[]);
void HandleClientTCP(int clntSocket);
char *random_quote(char *userFile);

int main()
{
    int listenfd, connfd, udpfd, nready, maxfdp1, quoteLikes[43];
    char buffer[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    //const int on = 1;
    struct sockaddr_in clntaddr, servaddr;
    char* message = "Henlo Client\n";
    void sig_chld(int);
    /* create listening TCP socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    
    // binding server addr structure to listenfd
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(bind < 0){
        DieWithError("bind() failed");
    }
    listen(listenfd, 10);
    
    /* create UDP socket */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    // binding server addr structure to udp sockfd
    bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(bind < 0)
    {
        DieWithError("bind() failed");
    }
    
    // clear the descriptor set
    FD_ZERO(&rset);
    
    // get maxfd
    maxfdp1 = max(listenfd, udpfd) + 1;
    for (;;) {
        
        // set listenfd and udpfd in readset
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        
        // select the ready descriptor
        nready = select(maxfdp1, &rset, NULL, NULL, NULL);
        
        // if tcp socket is readable then handle
        // it by accepting the connection
        if (FD_ISSET(listenfd, &rset)) { 
            len = sizeof(clntaddr);
            connfd = accept(listenfd, (struct sockaddr*)&clntaddr, &len);
            if ((childpid = fork()) == 0) { 
                close(listenfd);
                bzero(buffer, sizeof(buffer)); 
                printf("Message From TCP client: "); 
                read(connfd, buffer, sizeof(buffer)); 
                puts(buffer);
                // Checks for quote request from client
                // Sends quote of the day back
                if(strcmp(buffer, "Quote") == 0){
                    printf("Returning quote...\n");
                    // Checks quote for dislike
                    message = random_quote("Quote_File.txt");
                    int disliked = 0;
                    while(disliked != 1)
                    {
                        char subbuff[3];
                        memcpy(subbuff, &message[0], 2);
                        subbuff[2] = '\0';
                        int index = atoi(subbuff);
                        if (message[index] != 0) {
                            disliked = 1;
                            break;
                        }
                        else{
                            message = random_quote("Quote_File.txt");
                            continue;
                        }
                    }
                    message = CaesarCipher(1, message);
                    write(connfd, (const char*)message, sizeof(buffer));
                    // Listens for client's like/dislike response
                    close(listenfd);
                    bzero(buffer, sizeof(buffer));
                    printf("Message From TCP client: ");
                    read(connfd, buffer, sizeof(buffer));
                    puts(buffer);
                    // Sets array index representing quote line as 1 (like) or 0 (dislike)
                    // Checks last character bit for L (Like) or D (Dislike)
                    if(strcmp(&buffer[2], "L") == 0){
                        printf("Storing like...\n");
                        char subbuff[3];
                        memcpy( subbuff, &buffer[0], 2);
                        subbuff[2] = '\0';
                        int index = atoi(subbuff);
                        quoteLikes[index] = 1;
                        char *likeConf = "Liked\n";
                        write(connfd, (const char*)likeConf, sizeof(buffer));
                        close(connfd);
                        exit(0);                    }
                    else if(strcmp(&buffer[2], "D") == 0){
                        printf("Storing dislike...\n");
                        char subbuff[3];
                        memcpy( subbuff, &buffer[0], 2);
                        subbuff[2] = '\0';
                        int index = atoi(subbuff);
                        quoteLikes[index] = 0;
                        char *likeConf = "Disliked\n";
                        write(connfd, (const char*)likeConf, sizeof(buffer));
                        close(connfd);
                        exit(0);
                    }
                }
                
                // Checks for country info request
                else if(strcmp(buffer, "CC") == 0){
                    char id[6];
                    char code[4];
                    char name[30];
                    char continent[4];
                    char idBuffer[7];
                    printf("Country info request...\n");
                    char *confMsg = "Country info request received\n";
                    write(connfd, (const char*)confMsg, sizeof(buffer));
                    
                    // Listens for  country ID
                    close(listenfd);
                    bzero(buffer, sizeof(buffer));
                    printf("Message From TCP client: ");
                    read(connfd, buffer, sizeof(buffer));
                    puts(buffer);
                    printf("Received country ID\n");
                    strncpy(idBuffer, buffer, 6);
                    idBuffer[sizeof(idBuffer) - 1] = '\0'; // copies country ID into another buffer for later
                    char *confMsg2 = "Country ID received\n";
                    write(connfd, (const char*)confMsg2, sizeof(buffer));
                    
                    FILE *in_fp;
                    in_fp = fopen("Country_DB.csv","r");
                    if (in_fp == NULL) {
                        DieWithError("FOPEN() failed");
                    }
                    
                    while ((fgets(buffer,80,in_fp) != NULL)) {
                        strncpy(id,strtok(buffer,","),6);
                        strncpy(code,strtok(NULL,","),4);
                        strncpy(name,strtok(NULL,","),30);
                        strncpy(continent,strtok(NULL,","),4);
                        //note buffer is slightly changed with strtok
                        if(strcmp(idBuffer, id) == 0){
                            break;
                        }
                    }
                    
                    printf("Message From TCP client: ");
                    read(connfd, buffer, sizeof(buffer));
                    puts(buffer);
                
                    // checks for which information the client is requesting
                    if(strcmp(buffer, "K") == 0){
                        strncpy(buffer, code, 4);
                        // Sending the country code to the client
                        char *encryptedMsg = CaesarCipher(1, buffer);
                        printf("Sending code to client...\n");
                        write(connfd, (const char*)encryptedMsg, sizeof(buffer));
                        close(connfd);
                        exit(0);
                    }
                    else if(strcmp(buffer, "N") == 0){
                        strncpy(buffer, name, 30);
                        // Sending the country name to the client
                        char *encryptedMsg = CaesarCipher(1, buffer);
                        printf("Sending name to client...\n");
                        write(connfd, (const char*)encryptedMsg, sizeof(buffer));
                        close(connfd);
                        exit(0);
                    }
                    else if(strcmp(buffer, "C") == 0){
                        strncpy(buffer, continent, 4);
                        // Sending the country continent to the client
                        char *encryptedMsg = CaesarCipher(1, buffer);
                        printf("Sending continent to client...\n");
                        write(connfd, (const char*)encryptedMsg, sizeof(buffer));
                        close(connfd);
                        exit(0);
                    }

                }
                else {
                    close(connfd);
                    exit(0);
                }
            } 
            close(connfd); 
        } 
        // if udp socket is readable receive the message
        if (FD_ISSET(udpfd, &rset)) {
            len = sizeof(clntaddr);
            int hour;
            char am_or_pm;
            time_t current = time(NULL);
            bzero(buffer, sizeof(buffer));
            printf("\nMessage from UDP client: ");
            n = recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clntaddr, &len);
            if(recvfrom < 0){
                DieWithError("recvfrom() failed");
            }
            puts(buffer);
     
            // returns GMT formatted time to client
            if(strcmp(buffer, "GMT") == 0){
                struct tm *ptr;
                ptr = gmtime(&current);
                hour = ptr -> tm_hour;
                if (hour < 11)
                    am_or_pm = 'a';
                else {
                    hour -= 12;
                    am_or_pm = 'p';
                }
                if (hour == 0)
                    hour = 12;
                
                snprintf(buffer, sizeof(buffer), "%.02d-%.02d-%d %02d:%02d%c\n", ptr->tm_mon +1, ptr ->tm_mday, ptr->tm_year +1900, hour, ptr->tm_min, am_or_pm);
                sendto(udpfd, (const char*)buffer, sizeof(buffer), 0, (struct sockaddr*)&clntaddr, sizeof(clntaddr));
                if(sendto < 0){
                    DieWithError("sendto() failed");
                }
            }
            
            // returns local formatted time to client
            else if(strcmp(buffer, "Local") == 0){
                struct tm *ptr;
                ptr = localtime(&current);
                hour = ptr -> tm_hour;
                if (hour < 11)
                    am_or_pm = 'a';
                else {
                    hour -= 12;
                    am_or_pm = 'p';
                }
                if (hour == 0)
                    hour = 12;
                
                snprintf(buffer, sizeof(buffer), "%.02d-%.02d-%d %02d:%02d%c\n", ptr->tm_mon +1, ptr ->tm_mday, ptr->tm_year +1900, hour, ptr->tm_min, am_or_pm);
                sendto(udpfd, (const char*)buffer, sizeof(buffer), 0, (struct sockaddr*)&clntaddr, sizeof(clntaddr));
                if(sendto < 0){
                    DieWithError("sendto() failed");
                }
            }
           
        }
    }
}

