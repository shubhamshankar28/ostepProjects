#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/wait.h>
#include<unistd.h>

int main(int argc , char *argv[]) {
    // if(argc == 1){

    //     char ** searchPaths = (char **)malloc(sizeof(char *));
    //     searchPaths[0] = "/bin";
    //     while(1) {
    //         printf("wish> ");
            
    //         char *command = parseCommand();

    //         //findSearchPath(command , searchPaths);
    //         executeCommand(searchPaths[0] , command);
    //     }

    // }
    char *argf[3] = {"ls.c" , "/home"  , NULL};
    execv("/bin/ls" , argf);
    return 0;
}