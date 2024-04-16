#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1084

int main(int argc, int*argv[]){
    // int opt
    int i_flag = 0;        
    int l_flag = 0;
    int n_flag = 0;
    int q_flag = 0;
    
    for(int i = 1; i < argc; i++) 
    {
        if (argv[i][0] == '-'){
            break;
        }
        int flag = argv[i][1];
        switch (flag){
            case 'i': i_flag = 1; break;
            case 'l': l_flag = 1; break;
            case 'n': n_flag = 1; break;
            case 'q': q_flag = 1; break;
            default:
                printf(stderr, "usage: fgrep[-i][-l][-n][-q] pattern[file file ...] \n" );
                exit(EXIT_FAILURE);
        }
    }

    // Find Pattern
    int pattern_index = 1 + i_flag + l_flag + n_flag + q_flag;
    //char pattern[] = argv[pattern_index];
    
    // no file given
    int i = 1;
    if (i >= argc){
        printf(stderr, "usage: fgrep[-i][-l][-n][-q] pattern[file file ...] \n" );
        exit(EXIT_FAILURE);
    }

    printf("i_flag", i_flag);
    printf("l_flag", l_flag);
    printf("n_flag", n_flag);
    printf("q_flag", q_flag);
    //printf("pattern", pattern);
}