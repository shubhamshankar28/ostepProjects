#include<stdio.h>
#include<stdlib.h>
#include<string.h>








void appendCharacter( char e , char **source , int* current) {
    char *d = *source;
    d = (char *)realloc(d ,(*current)  + 1);
    d[(*current)-1] = e;
    d[(*current)] = '\0';
    (*current) = (*current) + 1;
    *source = d;
}


char *initializeEmptyString() {
    char *temp = (char *)(malloc(sizeof(char)));
    temp[0] = '\0';
    return temp;
}



char* processFile(char * file , int *ctr , char ** total , int *current) {
    
    
    char *encodedString = initializeEmptyString();
    
    FILE *fp;

    fp = fopen(file , "r");
    if(!fp) {
        return "-1";
    }
    


    char *line = NULL;
    size_t n  = 0;
    
    int d;

    while((d = getc(fp)) != -1) {
        appendCharacter(d, total , current);
    }

    free(line);
    fclose(fp);
    return "+1";
}



int main(int argc , char *argv[]) {
    if(argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }



    char * finalString = initializeEmptyString();
    char *temp;
    int current = 1;

    int counter = 0;
    for(int i=1; i<argc; ++i) {
        processFile(argv[i] , &counter , &finalString , &current);
        if((temp[0] == '-') && (temp[1] == '1')) {
            return (-1);
        }
    }


    int length = current-1;


    for(int i=0;i<length;++i) {
        int j=i;
        while((j<length) && (finalString[j] == finalString[i])) {
            j++;
        }
        int z = j-i;
        fwrite(&z , sizeof(int) , 1 , stdout);
        fwrite(&finalString[i] , sizeof(char) , 1, stdout);
        i=--j;
    }
    return 0;
}