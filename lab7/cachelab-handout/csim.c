// Luis Mendoza Ramirez
// Alspencer Omondi

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <math.h>


int TOTAL_USES = 0;

struct Line{
  int uses;
  int is_valid;
  unsigned long long tag;
  char *block;
};

struct Set{
  struct Line *lines;
};

struct Cache{
  struct Set *sets;
};

struct Cache makeCache(int sets, int associativity){
    struct Line line;
    struct Set set;
    struct Cache cache;


    cache.sets = (struct Set*) malloc(sizeof(struct Set) * sets);
    for (int i = 0; i < sets; i++){
        set.lines = (struct Line*) malloc (sizeof(struct Line) * associativity);
        cache.sets[i] = set;
        for (int j = 0; j < associativity; j ++){
            line.uses = 0;
            line.is_valid = 0;
            line.tag = 0;
            set.lines[j] = line;
        }
    }
    return cache;
}

int LRU(struct Set set, int associativity){
    int least_uses = set.lines[0].uses;
    int evicted_index = 0;

    for(int i = 1; i < associativity; i++){
        if(set.lines[i].uses < least_uses){
            least_uses = set.lines[i].uses;
            evicted_index = i;
        }
    }
    return evicted_index;
}


char * runCache(struct Cache cache, int sets, int associativity, int block_size, unsigned long long int address, int summary[]){
    unsigned long long int address_tag = address >> (sets + block_size);
    unsigned long long int set_index = (address << (64-(sets+block_size))) >> ((64-(block_size+sets))+block_size);

    struct Set set = cache.sets[set_index];
    struct Line line;
    char * result = NULL;
    int hit_found = 0;
    for (int i = 0; i < associativity; i++){
        line = set.lines[i];
        if(line.tag == address && line.is_valid){
            summary[0]++;
            hit_found = 1;
            result = "hit";
            TOTAL_USES++;
            cache.sets[set_index].lines[i].uses = TOTAL_USES;
            break;
        }
    }

    if(!hit_found){
        int set_full = 1;
        int next_empty_line;
        summary[1]++;
        for(int i=0;i<associativity;i++){
            if(set.lines[i].is_valid == 0){
                set_full = 0;
                next_empty_line = i;
                break;
            }
        }
        if(!set_full){
            set.lines[next_empty_line].tag = address_tag;
            set.lines[next_empty_line].is_valid = 1;
            TOTAL_USES++;
            cache.sets[set_index].lines[next_empty_line].uses = TOTAL_USES;
            result = "miss";
        }
        else{
            summary[2]++;
            result = "miss eviction";
            int lru = LRU(set, associativity);
            set.lines[lru].tag = address_tag;
            TOTAL_USES++;
            set.lines[lru].uses = TOTAL_USES;

        }



    }
    return result;
}


int main(int argc, char *argv[])
{
    int help_flag, verbos_flag = 0;
    int index_bits = 0;
    int associativity = 0;
    int block_bits = 0;
    int opt = 0;
    char *tracefile_name = NULL;
    char operation;
    unsigned long long int address = 0;
    int size = 0;
    int hits = 0;
    int misses = 0;
    int evictions = 0;
    int summary[3] = {hits, misses, evictions};

    while ((opt = getopt(argc, argv, "hvsEbt")) != -1) {
        switch(opt){
            case 'h': help_flag = 1; break;
            case 'v': verbos_flag = 1; break;
            case 's': index_bits = atoi(argv[optind]); break;
            case 'E': associativity = atoi(argv[optind]); break;
            case 'b': block_bits = atoi(argv[optind]); break;
            case 't': tracefile_name = argv[optind]; break;
            //default:
            //    printf(stderr, "usage: csim[-hv] -s <s> -E <E> -b <b> -t <tracefile> \n" );
            //    exit(EXIT_FAILURE);
        }
    }

    if (help_flag){
        printf("Usage: /csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
        printf("-h: Optional help flag that prints usage information\n");
        printf("-v: Optional verbose flag that displays trace information\n");
        printf("-s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
        printf("-E <E>: Associativity (number of lines per set)\n");
        printf("-b <b>: Number of block bits (B = 2^b is the block size)\n");
        printf("-t <tracefile>: Name of the valgrind trace to replay\n");
    }
    if(verbos_flag){

    }
    int sets = pow(2.0, index_bits);   // get Number of set by 2^s
	int block_size = pow(2.0, block_bits);  // get blockSize by 2^b

    struct Cache cache = makeCache(sets, associativity);

    FILE *tracefile = fopen(tracefile_name, "r");
   
    char *result = NULL;
    while(fscanf(tracefile, " %c %llx, %d", &operation, &address, &size)!=EOF){
        if(operation != 'I'){
            result = runCache(cache, sets, associativity, block_size, address, summary);
            hits = summary[0];
            misses = summary[1];
            evictions = summary[2];
            if(verbos_flag){
                printf("%c %llx, %d %s", operation, address, size, result);
            }
        }
        if(operation == 'M'){
            result = runCache(cache, sets, associativity, block_size, address, summary);
            hits = summary[0];
            misses = summary[1];
            evictions = summary[2];
            if(verbos_flag){
                printf("%c %llx, %d %s", operation, address, size, result);
            }
        }
    }

    printSummary(hits, misses, evictions);
    return 0;
}
