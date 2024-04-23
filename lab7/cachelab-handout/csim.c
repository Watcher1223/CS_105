// Luis Mendoza Ramirez
// Alspencer Omondi

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int help_flag, verbos_flag = 0;
    int index_bits = 0;
    int associativity = 0;
    int block_bits = 0;
    int opt = 0;
    char *tracefile = NULL;

    while ((opt = getopt(argc, argv, "hvsEbt")) != -1) {
        switch(opt){
            case 'h': help_flag = 1; break;
            case 'v': verbos_flag = 1; break;
            case 's': index_bits = atoi(argv[optind+1]); break;
            case 'E': associativity = atoi(argv[optind+1]); break;
            case 'b': block_bits = atoi(argv[optind+1]); break;
            case 't': tracefile = argv[optind+1]; break;
            //default:
            //    printf(stderr, "usage: csim[-hv] -s <s> -E <E> -b <b> -t <tracefile> \n" );
            //    exit(EXIT_FAILURE);
        }
    }

    printf("%d\n", help_flag);
    printf("%d\n", verbos_flag);
    printf("%d\n", index_bits);
    printf("%d\n", associativity);
    printf("%d\n", block_bits);
    printf("%s\n", tracefile);
    printSummary(0, 0, 0);
    return 0;
}
