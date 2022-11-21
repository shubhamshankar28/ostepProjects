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
    target = (char **)realloc(target , ((*current)+1)*sizeof(char *));
    target[(*current)-1] = add;
    (*current) = (*current) + 1;
    *source = target;
}


void incrementIndirectCharArray(char **add , char ****source , int *current) {
    char ***target = *source;
    target = (char ***)realloc(target , ((*current)+1)*sizeof(char **));
    target[(*current)-1] = add;
    (*current) = (*current) + 1;
    *source = target;
}


void incrementIntegerArray(int add , int **source , int *current) {
    int *target = *source;
    target = (int *)realloc(target , ((*current)+1)*sizeof(int));
    target[(*current)-1] = add;
    (*current) = (*current) + 1;
    *source = target;
}




char ** generateArguments(char **mainCommand, char *command, int *numberOfArguments , char ** redirectionFile, int *argumentError) {
    
    char ** args = (char **)malloc(sizeof(char *));
    int current = 1;

    int length = strlen(command);
    char *curArg;
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
                // printf("breaking\n");
                *redirectionFile = file;
                break;
            }
        }

        *numberOfArguments = (*numberOfArguments) + 1;
        int j=i;
        curArg = (char *)(malloc(sizeof(char)));
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
    //printf("-%s-\n" , *mainCommand);
    // for(int i=0;i<current-1;++i) {
    //     printf("%d , %s--\n" ,strlen(args[i]), args[i]);
    // }

    return args;

}




char * findSearchPath(char *mainCommand , char **searchPaths) {

    if(strlen(mainCommand) == 0)
        return NULL;
    int i=0;
    while(searchPaths[i] != NULL) {
        char *destination = (char *)(malloc(strlen(searchPaths[i])+1));
        strcpy(destination , searchPaths[i]);
        concatenate(&destination , "/");
        concatenate(&destination , mainCommand);

        if(access(destination , X_OK) == 0) {
            return destination;
        }
        i++;
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

char * formatInput(char *command) {

    char *cleanedCommand = (char *)malloc(sizeof(char));
    cleanedCommand[0] = 0;
    int position = 1;

    int index= 0;
    int length = strlen(command);
    while(index < length) {
        if((command[index] != ' ') && (command[index] != '\t')) {
            appendCharacter(command[index] , &cleanedCommand , &position);
            index++;
        }
        else {

            int toAppend = ((index != 0) ? 1 : 0);
            while((index < length) && ((command[index] == ' ') || (command[index] == '\t'))) {
                index++;
            }

            if(index == length) {
                toAppend = 0;
            }

            if(toAppend) {
                appendCharacter(' ' , &cleanedCommand , &position);
            }
        }
    }

    return cleanedCommand;
}


char ** generateParallelCommandList(char *command) {

    char ** args = (char **)malloc(sizeof(char *));
    int current = 1;

    int length = strlen(command);

    for(int i=0; i<length; ++i) {
        if((command[i] == '&') || (i==0)) {
            char *tempCommand = (char *)malloc(sizeof(char));
            tempCommand[0] = '\0';
            int position  = 1;
            int j=i+1;
            if(command[i] != '&') {
                j =i;
            }

            while((j<length) && (command[j] != '&')) {
                appendCharacter(command[j] , &tempCommand , &position);
                j++;    
            }

            incrementArray(tempCommand, &args, &current);
            
            if(j==length)
                break;
            i = --j;
        }
    }

    args[current-1] = NULL;
    return args;
}   



void executeCommands(char ***searchPathsSource, char **mainCommands , char ***arguments , char **redirectionFiles , int *numberOfArgument, int * argumentErrors , int numberOfCommands) {
    
    int *pidList = (int *)malloc(sizeof(int *));
    int size = 1;

    for(int i=0; i<numberOfCommands; ++i) {
        char *mainCommand = mainCommands[i];
        char **searchPaths = *searchPathsSource;
        char **args = arguments[i];
        int numberOfArguments = numberOfArgument[i];
        int length;
        char *redirectionFile = redirectionFiles[i];
        int argumentError = argumentErrors[i];

        if(argumentError == 1) {
            printError();
            continue;
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
            searchPaths = realloc(searchPaths , sizeof(char *)*(numberOfArguments));
            length = numberOfArguments - 1;
            for(int i=0;i<numberOfArguments-1;++i) {
                searchPaths[i] = args[i+1];
            }  
            searchPaths[numberOfArguments-1] = NULL;
            *searchPathsSource = searchPaths;
        }
        else {
            // printf("executing find search path\n");
            char *destination = findSearchPath(mainCommand , searchPaths);
            if(destination == NULL) {
                printError();
                continue;
            }
            else {
               int rc = fork();
               if(rc == 0) {

                    if(redirectionFile != NULL) {
                        int fd1 = creat(redirectionFile , 0644) ;
                        dup2(fd1, STDOUT_FILENO);
                        close(fd1);
                    }
                    execv(destination , args);
               }
               else {
                incrementIntegerArray(rc , &pidList , &size);
               }
            }
        }
    }



    for(int i=0;i<size-1;++i) {
        waitpid(pidList[i] , NULL , 0);
    }
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

    char ** searchPaths = (char **)malloc(sizeof(char *)*2);

    searchPaths[0] = "/bin";
    searchPaths[1] = NULL;
    while(1) {
        
        if(argc == 1)
            printf("wish> ");
        

        int eof = 0;
        char *command = ((argc == 1) ? parseCommand() : readCommand(&fp , &eof));


        if(eof == -1) {
            break;
        }

        char **rawCommandList = generateParallelCommandList(command);

        char **mainCommands = (char **)malloc(sizeof(char *));
        int commandSize = 1;
        char ***arguments = (char ***)malloc(sizeof(char **));
        int argumentSize = 1;
        int *numberOfArguments = (int *)malloc(sizeof(int *));
        int numberOfArgumentsSize = 1;

        char **redirectionFiles = (char **)malloc(sizeof(char *));
        int redirectFileSize = 1;
        int *argumentErrors = (int *)malloc(sizeof(int *));
        int argumentErrorsSize = 1;    


        int ind = 0;
        while(rawCommandList[ind] != NULL) {
            //printf("%s\n"  ,rawCommandList[ind]);
            rawCommandList[ind] = formatInput(rawCommandList[ind]);
            //printf("%s\n", rawCommandList[ind]);
            if(strlen(rawCommandList[ind]) == 0) {
                ind++;
                continue;
            }

            int numberOfArgument = 0; 
            char *mainCommand = (char *)malloc(sizeof(char));
            mainCommand[0] = '\0';
            char *redirectionFile = NULL;
            int argumentError  = 0;
            char **args = generateArguments(&mainCommand, rawCommandList[ind], &numberOfArgument, &redirectionFile , &argumentError);
            //printf("-%s-\n" , mainCommand);

            incrementArray(mainCommand , &mainCommands , &commandSize);
            incrementIndirectCharArray(args , &arguments, &argumentSize);
            incrementArray(redirectionFile , &redirectionFiles , &redirectFileSize);
            incrementIntegerArray(numberOfArgument , &numberOfArguments , &numberOfArgumentsSize);
            incrementIntegerArray(argumentError , &argumentErrors , &argumentErrorsSize);


            ind++;
        }   


        //printf("-%s- %d\n" , mainCommands[0] , commandSize);
        int numberOfCommands = commandSize-1;
    
        executeCommands(&searchPaths , mainCommands , arguments , redirectionFiles , numberOfArguments , argumentErrors , numberOfCommands);
    }
    return 0;
}