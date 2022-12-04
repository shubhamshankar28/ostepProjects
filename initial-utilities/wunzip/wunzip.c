#include<stdio.h>
#include<stdlib.h>
#include<string.h>









char* processFile(char * file) {
    
    
    FILE *fp;

    fp = fopen(file , "r");
    if(!fp) {
        return "-1";
    }
    


    char *line = NULL;
    size_t n  = 0;
    
    int d;
    int z;
    char c;
    // printf("entered\n");
    while((d = fread(&z , sizeof(int) , 1, fp)) != 0) {
       // printf("%d\n" , z);
        fread(&c , sizeof(char) , 1, fp);
        //printf("%c\n" , c);
        for(int i=0; i <  z; ++i) {
            printf("%c" , c);
        }
    }

    free(line);
    fclose(fp);
    return "+1";
}



int main(int argc , char *argv[]) {
    if(argc < 2) {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }




    char *temp;


    int counter = 0;
    for(int i=1; i<argc; ++i) {
        // printf("in here\n");
        temp = processFile(argv[i]);
        if((temp[0] == '-') && (temp[1] == '1')) {
            return (-1);
        }
    }



    return 0;
}