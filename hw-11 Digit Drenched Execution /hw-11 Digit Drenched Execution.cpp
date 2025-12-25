#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
int do_command(const char* command) {
    if (command == NULL) {
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        return -1; 
    }
    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, (char*)NULL);
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        return -1;
    }
    return status;
}
int main(){
    return 0;
}
