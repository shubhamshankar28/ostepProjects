#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/wait.h>
#include<unistd.h>

int writeFile(char * s) {
    FILE *fp = fopen(s , "r");
    if(!fp) {
        return -1;
    }

    char *line = NULL;
    size_t n  = 0;
    while(getline(&line , &n , fp) != -1) {
        printf("%s" ,  line);
    }

    free(line);
    return 1;
}


char *parseCommand() {
    char *line = NULL;
    size_t n = 0;
    getline(&line , &n, stdin);

    int len = strlen(line);
    char *another = (char *)malloc(len);
    
    for(int i=0;i<len-1;++i) {
        another[i] = line[i];
    }
    another[len-1] = '\0';
    free(line);
    return another;
}


void concatenate(char **dest , char *source) {
    // printf("%s" , *dest);
    *dest = realloc(*dest, strlen(*dest) + strlen(source) + 1);
    strcat(*dest , source);
}


void appendCharacter( char e , char **source , int* current) {
    char *d = *source;
    d = (char *)realloc(d ,(*current)  + 1);
    d[(*current)-1] = e;
    d[(*current)] = '\0';
    (*current) = (*current) + 1;
    *source = d;
}


void incrementArray(char *add , char ***source , int *current) {
    char **target = *source;
    target = (char **)realloc(target , (*current)+1);
    target[(*current)-1] = add;
    (*current) = (*current) + 1;
    *source = target;
}

char ** generateArguments(char **mainCommand, char *command) {
    

    char ** args = (char **)malloc(sizeof(char *));
    int current = 1;

    int length = strlen(command);
    for(int i=0;i<length;++i) {

        int j=i;
        char *curArg = (char *)(malloc(sizeof(char)));
        curArg[0] = '\0';
        int curPos = 1;

        
        while((j<length) && (command[j] != ' ')) {
            appendCharacter( command[j] , &curArg, &curPos);
            j++;
        }


        if(i==0) {
            *mainCommand = curArg;
        }

        incrementArray(curArg , &args , &current);
        i=j;
    }

    args[current-1] = NULL;

    // for(int i=0;i<current-1;++i) {
    //     printf("%d , %s--\n" ,strlen(args[i]), args[i]);
    // }


    return args;

}
void executeCommand(char *searchPath , char *command) {
    int rc = fork();

    if(rc == 0) {
        char *destination = (char *)(malloc(strlen(searchPath)+1));
        strcpy(destination , searchPath);
        concatenate(&destination , "/");


        char *mainCommand = (char *)(malloc(sizeof(char)));
        char **args = generateArguments(&mainCommand , command);

        concatenate(&destination , mainCommand);
        

        execv(destination , args);
    }
    else {
        wait(NULL);
    }
}
int main(int argc , char *argv[]) {
    if(argc == 1){

        char ** searchPaths = (char **)malloc(sizeof(char *));
        searchPaths[0] = "/bin";
        while(1) {
            printf("wish> ");
            
            char *command = parseCommand();

            //findSearchPath(command , searchPaths);
            executeCommand(searchPaths[0] , command);
        }

    }
    return 0;
}