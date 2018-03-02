//
//  main.c
//  fork
//
//  Created by Avishek Mondal on 26/2/18.
//  Copyright © 2018 Avishek Mondal. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int count, char **args) {
    pid_t id1;
    pid_t id2;
    id1= fork();
    printf("id1 = %d, getpid()= %d, getppid()=%d \n", id1,getpid(),getppid());
    id2=fork();
    printf("id2 = %d, getpid()= %d, getppid()=%d \n", id2,getpid(),getppid());
//    if(getpid()==0) printf("Pid is %d, ppid is %d\n",getpid(),getppid());
//    else printf("id is %d\n", getpid());
    return 0;
}
