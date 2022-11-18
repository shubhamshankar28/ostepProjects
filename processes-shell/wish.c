#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/wait.h>
#include<unistd.h>



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

char ** generateArguments(char **mainCommand, char *command, int *numberOfArguments , char ** redirectionFile, int *argumentError) {
    

    char ** args = (char **)malloc(sizeof(char *));
    int current = 1;

    int length = strlen(command);
    for(int i=0;i<length;++i) {


        if(command[i] == '>') {
            int next = i;

            while((next < length) && (command[next] != ' ')) {
                next++;
            }
            if(next != (i+1)) {
                *argumentError = 1;
                return args;
            }

            next++;

            if(next == length) {
                *argumentError = 1;
                return args;
            }     

            char *file = (char *)(malloc(sizeof(char)));
            file[0] = '\0';
            int curPos = 1;  
            while((next<length) && (command[next] != ' ')) {
                appendCharacter(command[next] , &file , &curPos);
                next++;
            } 

            if(next != length) {
                *argumentError = 1;
                return args;
            }
            else {
                *redirectionFile = file;
                break;
            }
        }

        *numberOfArguments = (*numberOfArguments) + 1;
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
void executeCommand(char *destination, char **args , char *redirectionFile) {
    int rc = fork();

    if(rc == 0) {

        if(redirectionFile != NULL) {
            int fd1 = creat(redirectionFile , 0644);
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
        }

        execv(destination , args);
    }
    else {
        wait(NULL);
    }
}

char * findSearchPath(char *mainCommand , char **searchPaths , int len) {
    
    for(int i=0; i<len; ++i) {
        char *destination = (char *)(malloc(strlen(searchPaths[i])+1));
        strcpy(destination , searchPaths[i]);
        concatenate(&destination , "/");
        concatenate(&destination , mainCommand);

        if(access(destination , X_OK) == 0) {
            return destination;
        }
    }

    return NULL;
}

void printError() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

char *readCommand(FILE ** fp , int *error) {
    char *line = NULL;
    size_t n  = 0;
    *error = getline(&line , &n , *fp);
    if((*error) == -1)
        return line;
    
    int len = strlen(line);

    if(line[len-1] != '\n') {
        return line;
    }
    char *another = (char *)malloc(len);
    
    for(int i=0;i<len-1;++i) {
        another[i] = line[i];
    }
    another[len-1] = '\0';
    free(line);
    return another;

}
int main(int argc , char *argv[]) {


    if(argc > 2) {
        printError();
        exit(1);
    }


    FILE *fp = NULL;

    if(argc == 2) {
        fp = fopen(argv[1] , "r");
        if(!fp) {
            printError();
            exit(1);
        }
    }

    char ** searchPaths = (char **)malloc(sizeof(char *));
    int length = 1;
    searchPaths[0] = "/bin";
    while(1) {
        
        if(argc == 1)
            printf("wish> ");
        

        int eof = 0;
        char *command = ((argc == 1) ? parseCommand() : readCommand(&fp , &eof));
        if(eof == -1) {
            break;
        }
        int numberOfArguments = 0; 
        char *mainCommand = (char *)malloc(sizeof(char));
        char *redirectionFile = NULL;

        int argumentError  = 0;
        char **args = generateArguments(&mainCommand, command, &numberOfArguments, &redirectionFile , &argumentError);

        if(argumentError == 1) {
            printError();
            exit(0);
        }

        if(strcmp(mainCommand , "exit") == 0) {
            if(numberOfArguments != 1) {
                printError();
                exit(0);
            }
            exit(0);
        
        }
        else if(strcmp(mainCommand , "cd") == 0) {
            if(numberOfArguments != 2) {
                printError();
                exit(0);
            }
            chdir(args[1]);
        }
        else if(strcmp(mainCommand, "path") == 0) {
            searchPaths = realloc(searchPaths , sizeof(char *)*(numberOfArguments-1));
            length = numberOfArguments - 1;
            for(int i=0;i<numberOfArguments-1;++i) {
                searchPaths[i] = args[i+1];
            }  
        }
        else {
            char *destination = findSearchPath(mainCommand , searchPaths, length);
            if(destination == NULL) {
                printError();
                continue;
            }
            else {
                executeCommand(destination, args, redirectionFile);
            }
        }
    }
    return 0;
}