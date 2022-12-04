#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void printIf(char *term , char *line) {    
    if(strlen(term) == 0) {
        printf("%s" , line);
        return ;
    }
    int lenTerm = strlen(term) , lenLine = strlen(line);

    for(int i = 0; i < lenLine - lenTerm + 1; ++i) {
        int equal = 1;
        for(int j=i; j < i + lenTerm;++j) {
            if(line[j] != term[j-i]){ 
                equal = 0;
                break;
            }
        }
        if(equal == 1) {
            printf("%s" , line);
            return ;
        }
    }
}

int processFile(char *term , char * file , FILE *f) {
    FILE *fp;
    if(f == NULL) {
        fp = fopen(file , "r");
        if(!fp) {
            return -1;
        }
    }
    else {
        fp = f;
    }

    char *line = NULL;
    size_t n  = 0;
    while(getline(&line , &n , fp) != -1) {
        printIf(term, line);
    }

    free(line);
    fclose(fp);
    return 1;
}



int main(int argc , char *argv[]) {
    if(argc < 2) {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }

    for(int i=2; i<argc; ++i) {
        if(processFile(argv[1], argv[i] , NULL) == -1) {
            printf("wgrep: cannot open file\n");
            return 1;
        }
    }

    if(argc == 2)
        processFile(argv[1] ," ", stdin);   
    return 0;
}