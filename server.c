// SERVER
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <utmp.h>

int main() {

    if (access("Canal1", F_OK) == -1) { //Accesam fisierul Canal1 si daca nu exista atunci
        if (mknod("Canal1", S_IFIFO | 0666, 0) == -1) { //vom crea Canal1 si vom verifica potentialele erori
            perror("fifo");
            exit(EXIT_FAILURE);
        }
    }

    int fd_client_server; //Comenzi din client 
    if ((fd_client_server = open("Canal1", O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////
    if (access("Canal2", F_OK) == -1) { 
        if (mknod("Canal2", S_IFIFO | 0666, 0) == -1) { 
            perror("fifo");
            exit(EXIT_FAILURE);
        }
    }
    int fd_server_client;
    if ((fd_server_client = open("Canal2", O_WRONLY)) == -1) {
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
    if ((fd_login = open("Canal3", O_WRONLY)) == -1) {
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
    if ((fd_login2 = open("Canal4", O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }


    char buffer[255]; 
    int citit = 0;
    int login_conectat = 0; 
    do 
    {
        citit = read(fd_client_server, buffer, 255);
        buffer[citit] = '\0'; 

        const char* logout_mesaj = "V-ati delogat de pe contul dumneavoastra.";
        const char* else_mesaj = "Comanda introdusa este invalida";
        const char* login_mesaj = "Nume Valid";
        const char* login_mesaj_invalid = "Nume Valid";
        const char* get_proc_info_mesaj = "Procesul nu exista.";

        if(strcmp(buffer,"logout")==0)
        {
            //citit = read(fd_login2,buffer,255);
            //buffer[citit]='\0';
            //login_conectat = atoi(buffer);
            //printf("%d",login_conectat);
            if(login_conectat)
            {
                login_conectat=0;
                write(fd_server_client,logout_mesaj,strlen(logout_mesaj)); 
            }
            else
            {
                write(fd_server_client,logout_mesaj,strlen(logout_mesaj)); 
            }

            
        }
        else if(strstr(buffer,"login : ")) // De forma "login : "
        {   
                int pipes[2];  
                pipe(pipes);
            
                pid_t child = fork();

                if(child > 0)
                { // parent
                    close(pipes[1]);

                    char aux[256]; aux[0]='\0'; int auxCitit; auxCitit=read(pipes[0],aux,256);

                    aux[auxCitit]='\0';
                    if(strcmp(aux,login_mesaj)==0)
                        login_conectat=1;


                    write(fd_server_client,aux,auxCitit);
                }
                else 
                { // child
                    close(pipes[0]);

                    char *numeleMeu=strtok(buffer," ");
                
                    for(int i=1;i<=2;i++)
                        numeleMeu=strtok(NULL," \n"); 

                    int fd_usernames=open("usernames.txt",O_RDONLY); // Vom folosi fd_usernames pentru a face legatura cu fisierul cu usernames
                    char numeCitite[4096]; numeCitite[0]='\0';

                    citit=read(fd_usernames,numeCitite,4096);
                    numeCitite[citit]='\n'; 

                    char *numeDinFisier=strtok(numeCitite,"/");
                    int numeGasit=0;

                    while(numeDinFisier!=NULL && numeGasit==0)
                    {   
                        if(numeDinFisier!=NULL)
                            if(strcmp(numeleMeu,numeDinFisier)==0)
                            {
                                write(pipes[1],login_mesaj,strlen(login_mesaj));
                                numeGasit=1;
                                write(fd_login,"1",1);
                            }
                        numeDinFisier=strtok(NULL,"/");
                    }

                    close(fd_usernames);

                    if(numeGasit==0)
                        write(pipes[1],login_mesaj_invalid,strlen(login_mesaj_invalid));
                }
                
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////
        else if(strcmp(buffer,"get-logged-users")==0)
        {
            int sockets[2],child;
            socketpair(AF_UNIX,SOCK_STREAM,0,sockets);
            int child2 = fork();

            if(child2== 0)
            {
                close(sockets[1]);
                struct utmp *p;
                p = getutent();
                    write(sockets[0],p->ut_user,strlen(p->ut_user));
                    write(sockets[0],"\n",1);
                    write(sockets[0],p->ut_host,strlen(p->ut_host));
                    write(sockets[0],"\n",1);
                    printf("%i\n",p->ut_tv.tv_sec);

                
                close(sockets[0]);
            }
            else{
                close(sockets[0]);
                char socketPrimit[4096];
                socketPrimit[0]='\0';

                int socket_primit_lungime = read(sockets[1],socketPrimit,4096);
                write(fd_server_client,socketPrimit,socket_primit_lungime);

                close(sockets[1]);
            }

            
        }    
            ////////////////////////////////////////////////////////////////////////////////////////////////////
        else if(strstr(buffer,"get-proc-info : ")) // De forma "get-proc-info : "
        {
            int myPipe[2];  
            pipe(myPipe);
        
            pid_t child = fork();

            if(child > 0)
            { // parent
                close(myPipe[1]);

                char aux[256]; aux[0]='\0'; int auxL; auxL=read(myPipe[0],aux,256);

                aux[auxL]='\0';

                write(fd_server_client,aux,auxL);
            }
            else 
            { // child
                close(myPipe[0]);

                //if(log == 1)
                {
                    char *childComanda = strtok(buffer," ");
                    for(int i=1;i<=2;i++)
                    {
                        //printf("%s\n",childComanda);
                        childComanda=strtok(NULL," \n"); 
                    } 
                    
                    printf("%s\n",childComanda);

                    char sursa[50]; sursa[0]='\0';
                    strcat(sursa,"/");
                    strcat(sursa,"proc");
                    strcat(sursa,"/");
                    strcat(sursa,childComanda);
                    strcat(sursa,"/");
                    strcat(sursa,"status");

                    int fd_usernames;
                    if((fd_usernames = open(sursa,O_RDONLY)) == -1)
                       write(myPipe[1],get_proc_info_mesaj,strlen(get_proc_info_mesaj));

                    char infoProc[1000]; infoProc[0] = '\0'; int index; index = read(fd_usernames,infoProc,1000); infoProc[index]='\0';

                    do 
                    {
                        char *pointer = strtok(infoProc,"\n");
                        while(pointer)
                        {
                            if(strstr(pointer,"Name")!=0 || strstr(pointer,"State")!=0 || strstr(pointer,"Ppid")!=0 || strstr(pointer,"Vmsize")!=0 || strstr(pointer,"Uid")!=0)  //de adaugat campurile lipsa
                            {
                                write(myPipe[1],pointer,strlen(pointer));
                                write(myPipe[1],"\n",1);
                            }
                            pointer=strtok(NULL,"\n");
                        }
                        index = read(fd_usernames,infoProc,1000); infoProc[index]='\0';

                    }while(index);
                }
                //else write(myPipe[1],"Nu sunteti autentificat",strlen("Nu sunteti autentificat"));
            }        
        }
        else write(fd_server_client,else_mesaj,strlen(else_mesaj));
    } while(citit);    

    close(fd_client_server);
    close(fd_server_client);
}