#include<stdio.h>
#include<stdlib.h>

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

int main(int argc , char *argv[]) {
    for(int i=1; i<argc; ++i) {
        if(writeFile(argv[i]) == -1) {
            printf("wcat: cannot open file\n");
            return 1;
        }
    }
    return 0;
}