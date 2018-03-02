#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <pthread.h>

#define DEBUG

int socket_fd, client_sock;
struct sockaddr_in server, client;

void signalHandler( int signum )
{
    printf("Server interrupted with signal (%d). Shutting down\n",signum);
    
    // cleanup and close up stuff here
    // terminate program
    close(socket_fd);
    
    exit(signum);
    
}

void *f(void *arg) {
    int client_sock = *(int *)arg;
    int command_length;
    char *command;
    int p[2];
    
    free(arg);
    
    /* first receive an int indicating the length of the command to run */
    if(recv(client_sock, &command_length, sizeof(int), 0) != sizeof(int)) {
        perror("Server (thread) receipt of message length failed");
        close(client_sock);
        pthread_exit(NULL);
    }
#ifdef DEBUG
    printf("Server (thread %d) told to expect command of length %d\n",client_sock,command_length);
#endif
    
    
    if( command_length > 0) {
        int total_received = 0;
        int received_now = 0;
        
        
        
        /* get the command from the client */
#ifdef DEBUG
        printf("Server (thread %d) allocating space for comand from client of length %d\n",client_sock,command_length);
#endif
        command = malloc((command_length+1)*sizeof(char));
        
        total_received = 0;
        received_now = 0;
        while(total_received != command_length) {
            received_now= recv(client_sock,
                               command+total_received*sizeof(char),
                               command_length-total_received , 0);
            if(received_now < 0) {
                perror("Server (thread) receive failed");
                free(command);
                close(client_sock);
                pthread_exit(NULL);
            }
            total_received +=received_now;
        }
#ifdef DEBUG
        printf("Server (thread %d) received command %s from client\n", client_sock, command);
#endif
        
        /* we need to create a pipe, fork, execl the command, wait
         for the process to terminate, consume the pipe, and
         write the return to the client, easy */
        pipe(p);
        
        if(fork() == 0) {
            close(p[0]);
            
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            if(dup(p[1]) == -1) {
                perror("Server (thread; forked child) Couldn't redirect stdout to pipe");
                exit(-1);
            }
            if(dup(p[1]) == -1) {
                perror("Server (thread; forked child) Couldn't redirect stderr to pipe");
                exit(-1);
            }
            execl(command,command,NULL,NULL);
        } else {
            char buf[1024];
            ssize_t nr = 0;
            close(p[1]);
            
            // nr num read
            while((nr = read(p[0],buf,1024))>0) {
                int nw = 0; // number written
                int tw = 0; // total written
                
                while(tw < nr) {
                    nw = send(client_sock, buf+tw*sizeof(char) , nr-tw,0);
                    if (nw < 0) {
                        perror("Error writing return message, shutting server down");
                        close(client_sock);
                        pthread_exit(NULL);
                    }
                    tw+=nw;
#ifdef DEBUG
                    printf("Server (thread %d) wrote %d bytes (return = %s) to client socket\n", client_sock, nw, buf);
#endif
                }
                
                
                
            }
        }
        free(command);
    }
    
    close(client_sock);
    pthread_exit(0); // this is a gross use of a ptr to return a value
}




int main(int argc , char *argv[])
{
    
    int    c;
    pthread_t thread;
    
    /* register signal handler */
    signal(SIGINT, signalHandler);
    
    /* Initialize socket */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Socket not created.");
        exit(1);
    }
    puts("Socket created");
    
    /* Prepare sockaddr_in structure */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    
    /* Bind socket to port */
    if(bind(socket_fd,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Socket bind failed. Error");
        exit(1);
    }
    puts("Server socket bound to port 8888");
    
    /* Indicate that we will be listening on this socket */
    listen(socket_fd , 3);
    
    /* prepare to accept incoming connection */
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    while(1) {
        int *client_connection_fd ;
        
        /* accept connection from an incoming client */
        client_sock = accept(socket_fd, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            exit(1);
        }
#ifdef DEBUG
        puts("Connection accepted");
#endif
        // allocate space to pass client socket fd to processing thread
        client_connection_fd = malloc(sizeof(int));
        *client_connection_fd = client_sock;
        
        /* spawn thread to deal with request */
        pthread_create(&thread,0,f,client_connection_fd);
        
    }
    
    return 0;
}
