//
//  fork_example.c
//  fork
//
//  Created by Avishek Mondal on 27/2/18.
//  Copyright © 2018 Avishek Mondal. All rights reserved.
//

#include "fork_example.h"

pid_t id;

id = fork();
if(id == 0) {
    printf("I'm (%d) the child of (%d).\n", getpid(), getppid());
    sleep(1);
} else {
    printf("I'm (%d) the parent of (%d).\n", getpid(), id);
    sleep(1);
}
