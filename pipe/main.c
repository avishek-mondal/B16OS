//
//  main.c
//  pipe
//
//  Created by Avishek Mondal on 28/2/18.
//  Copyright © 2018 Avishek Mondal. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

int times2(int d) {
    return d*2;
}

int main(int argc, char **argv) {
    
    int p[2];
    pipe(p);
    
    if(fork() == 0) {
        char mesg[] = "Hello from your only child.";
        ssize_t len=0;
        ssize_t tlen=0;
        close(p[0]);
        while ((len=write(p[1],mesg+tlen*sizeof(char),strlen(mesg)-tlen)) > 0)
            tlen +=len;
        
    } else {
        char mesg[120];
        ssize_t nr = 0;
        close(p[1]);
        
        while((nr = read(p[0],mesg,120))>0) {
            write(STDOUT_FILENO,mesg,nr);
        }
        write(STDOUT_FILENO,"\n",1);
    }
    return 0;
}
