// CLIENT
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <utmp.h>

int main() {
    
    if (access("Canal1", F_OK) == -1) { //if fifo does not exist    
        if (mknod("Canal1", S_IFIFO | 0666, 0) == -1) { //we create fifo and error-check
            perror("fifo");
            exit(EXIT_FAILURE);
        }
    }

    int fd_client_server; //file descriptor  
    if ((fd_client_server = open("Canal1", O_WRONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////
    if (access("Canal2", F_OK) == -1) { //if fifo does not exist
        if (mknod("Canal2", S_IFIFO | 0666, 0) == -1) { //we create fifo and error-check
            perror("fifo");
            exit(EXIT_FAILURE);
        }
    }
    
    int fd_server_client;
    if ((fd_server_client = open("Canal2", O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////
    if (access("Canal3", F_OK) == -1) { //if fifo does not exist
        if (mknod("Canal3", S_IFIFO | 0666, 0) == -1) { //we create fifo and error-check
            perror("fifo");
            exit(EXIT_FAILURE);
        }
    }
    int fd_login;
    if ((fd_login = open("Canal3", O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////
    if (access("Canal4", F_OK) == -1) { //if fifo does not exist
        if (mknod("Canal4", S_IFIFO | 0666, 0) == -1) { //we create fifo and error-check
            perror("fifo");
            exit(EXIT_FAILURE);
        }
    }
    int fd_login2;
    if ((fd_login2 = open("Canal4", O_WRONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }



    char buffer[255]; buffer[0] = '\0';
    int citit=0;

    while(citit != -1) { 

        citit = read(0,buffer,255);
        buffer[citit-1]='\0'; 


        while(strcmp(buffer,"quit")!=0)
        {
            write(fd_client_server, buffer, strlen(buffer));
            int citit = read(fd_server_client,buffer,255);
    
            buffer[citit]='\0';

            printf("%s\n",buffer);

            citit = read(0,buffer,1255);
            buffer[citit-1]='\0'; 
        }
        
        citit=-1;         
    }
    close(fd_client_server);
    close(fd_server_client);
}