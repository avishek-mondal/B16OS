#include<stdio.h> /*printf*/
#include<stdlib.h> /*printf*/
#include<string.h>    /*strlen*/
#include<sys/socket.h>    /*socket*/
#include<arpa/inet.h> /*inet_addr*/
#include <unistd.h>
#include<signal.h>

#define DEBUG

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char buf[1024];
    
    char *command = argv[2];
    char *host = argv[1];
    
    if(argc < 2) {
        puts("Usage: client <inet addr> <command>");
        exit(1);
    }
    
#ifdef DEBUG
    printf("(Client) host %s ; command %s\n",host, command);
#endif
    
    
    
    /*Create socket*/
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        perror("Could not create socket");
        exit(1);
    }
#ifdef DEBUG
    puts("(Client) Socket created");
#endif
    
    server.sin_addr.s_addr = inet_addr(host);
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
    
    /*Connect to "remote" server*/
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        exit(1);
    }
#ifdef DEBUG
    puts("(Client) Connected\n");
#endif
    
    /*communicate once with server*/
    int command_length = strlen(command);
    int total_sent = 0;
    int sent_now = 0;
    int received_now = 0;
    
    /*Send message length as int*/
    if( send(sock, &command_length, sizeof(int), 0) != sizeof(int)) {
        perror("Send of command length failed");
        return 1;
    }
    
    if(command_length ==0 ) {
        close(sock);
        return 0;
    }
    
    /*Send message as mesg length chars*/
    while(total_sent != command_length)
    {
        sent_now= send(sock , command+total_sent*sizeof(char), command_length-total_sent , 0);
        if(sent_now < 0) {
            perror("Send of command to server failed");
            return 1;
        }
        total_sent +=sent_now;
    }
    
#ifdef DEBUG
    puts("(Client) Command sent; reading back response.\n");
#endif
    
    
    
    /*Receive message reply from the server*/
    while((received_now=recv(sock, buf, 1024 , 0))!=0)
    {
#ifdef DEBUG
        printf("(Client) received %d bytes in response from server.\n",received_now);
#endif
        
        write(STDOUT_FILENO,buf,received_now);
        fflush(stdout);
    }
    
    
    
    close(sock);
    return 0;
}
