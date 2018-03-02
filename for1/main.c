#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    
    pid_t id;
    
    id = fork();
    if(id == 0) {
        int executable_return_value = -1;
        printf("I'm (%d) the child of (%d).  I'm going to run the compiled program primes.\n", getpid(), getppid());
        
        close(STDOUT_FILENO);
        if(open("/tmp/prime.stdout", O_WRONLY | O_CREAT) == -1) {
            perror("Couldn't redirect stdout to /dev/null in child.\n");
            exit(-1);
        }
        
        execl("./primes_example","primes","19",NULL);
        printf("Exit value from primes program %d\n",executable_return_value);
        exit(executable_return_value);
    } else {
        int child_return_value;
        printf("I'm (%d) the parent of (%d). I'm going to wait for the child to return before exiting.\n", getpid(), id);
        waitpid(id, &child_return_value, 0);
        printf("Child returned %d\n",WEXITSTATUS(child_return_value));
    }
    return 0;
}
