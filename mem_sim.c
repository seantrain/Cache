/***************************************************************************
 * *    Inf2C-CS Coursework 2: Cache Simulation
 * *
 * *    Instructor: Boris Grot
 * *
 * *    TA: Siavash Katebzadeh
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
/* Do not add any more header files */

/*
 * Various structures
 */
typedef enum {FIFO, LRU, Random} replacement_p;

const char* get_replacement_policy(uint32_t p) {
    switch(p) {
    case FIFO: return "FIFO";
    case LRU: return "LRU";
    case Random: return "Random";
    default: assert(0); return "";
    };
    return "";
}

typedef struct {
    uint32_t address;
} mem_access_t;

// These are statistics for the cache and should be maintained by you.
typedef struct {
    uint32_t cache_hits;
    uint32_t cache_misses;
} result_t;


/*
 * Parameters for the cache that will be populated by the provided code skeleton.
 */

replacement_p replacement_policy = FIFO;
uint32_t associativity = 0;
uint32_t number_of_cache_blocks = 0;
uint32_t cache_block_size = 0;


/*
 * Each of the variables below must be populated by you.
 */
uint32_t g_num_cache_tag_bits = 0;
uint32_t g_cache_offset_bits= 0;
result_t g_result;


/* Reads a memory access from the trace file and returns
 * 32-bit physical memory address
 */
mem_access_t read_transaction(FILE *ptr_file) {
    char buf[1002];
    char* token = NULL;
    char* string = buf;
    mem_access_t access;

    if (fgets(buf, 1000, ptr_file)!= NULL) {
        /* Get the address */
        token = strsep(&string, " \n");
        access.address = (uint32_t)strtol(token, NULL, 16);
        return access;
    }

    /* If there are no more entries in the file return an address 0 */
    access.address = 0;
    return access;
}

void print_statistics(uint32_t num_cache_tag_bits, uint32_t cache_offset_bits, result_t* r) {
    /* Do Not Modify This Function */

    uint32_t cache_total_hits = r->cache_hits;
    uint32_t cache_total_misses = r->cache_misses;
    printf("CacheTagBits:%u\n", num_cache_tag_bits);
    printf("CacheOffsetBits:%u\n", cache_offset_bits);
    printf("Cache:hits:%u\n", r->cache_hits);
    printf("Cache:misses:%u\n", r->cache_misses);
    printf("Cache:hit-rate:%2.1f%%\n", cache_total_hits / (float)(cache_total_hits + cache_total_misses) * 100.0);
}

/*
 *
 * Add any global variables and/or functions here as needed.
 *
 */


int main(int argc, char** argv) {
    time_t t;
    /* Intializes random number generator */
    /* Important: *DO NOT* call this function anywhere else. */
    srand((unsigned) time(&t));
    /* ----------------------------------------------------- */
    /* ----------------------------------------------------- */

    /*
     *
     * Read command-line parameters and initialize configuration variables.
     *
     */
    int improper_args = 0;
    char file[10000];
    if (argc < 6) {
        improper_args = 1;
        printf("Usage: ./mem_sim [replacement_policy: FIFO LRU Random] [associativity: 1 2 4 8 ...] [number_of_cache_blocks: 16 64 256 1024] [cache_block_size: 32 64] mem_trace.txt\n");
    } else  {
        /* argv[0] is program name, parameters start with argv[1] */
        if (strcmp(argv[1], "FIFO") == 0) {
            replacement_policy = FIFO;
        } else if (strcmp(argv[1], "LRU") == 0) {
            replacement_policy = LRU;
        } else if (strcmp(argv[1], "Random") == 0) {
            replacement_policy = Random;
        } else {
            improper_args = 1;
            printf("Usage: ./mem_sim [replacement_policy: FIFO LRU Random] [associativity: 1 2 4 8 ...] [number_of_cache_blocks: 16 64 256 1024] [cache_block_size: 32 64] mem_trace.txt\n");
        }
        associativity = atoi(argv[2]);
        number_of_cache_blocks = atoi(argv[3]);
        cache_block_size = atoi(argv[4]);
        strcpy(file, argv[5]);
    }
    if (improper_args) {
        exit(-1);
    }
    assert(number_of_cache_blocks == 16 || number_of_cache_blocks == 64 || number_of_cache_blocks == 256 || number_of_cache_blocks == 1024);
    assert(cache_block_size == 32 || cache_block_size == 64);
    assert(number_of_cache_blocks >= associativity);
    assert(associativity >= 1);

    printf("input:trace_file: %s\n", file);
    printf("input:replacement_policy: %s\n", get_replacement_policy(replacement_policy));
    printf("input:associativity: %u\n", associativity);
    printf("input:number_of_cache_blocks: %u\n", number_of_cache_blocks);
    printf("input:cache_block_size: %u\n", cache_block_size);
    printf("\n");

    /* Open the file mem_trace.txt to read memory accesses */
    FILE *ptr_file;
    ptr_file = fopen(file,"r");
    if (!ptr_file) {
        printf("Unable to open the trace file: %s\n", file);
        exit(-1);
    }

    /* result structure is initialized for you. */
    memset(&g_result, 0, sizeof(result_t));
    /* Do not delete any of the lines below.
     * Use the following snippet and add your code to finish the task. */

    /* You may want to setup your Cache structure here. */

    mem_access_t access;

    //calculating number of sets
    int setCount = number_of_cache_blocks / associativity;
    //block structure declaration
    struct Block_
    {
        int tag;
        long int accessed;
        int offset;
    };
    //set structure declaration
    struct Sets_
    {
        struct Block_ *blocks;
    };
    //cache structure declaration
    struct Cache_
    {
        int cache_hits;
        int cache_misses;
        struct Sets_ *sets;
    };
    //initializing cache
    struct Cache_ *cache;
    //setting size of cache
    cache = (struct Cache_ *) malloc (sizeof(struct Cache_));
    //initializing cache_hits and misses to 0
    cache->cache_hits = 0;
    cache->cache_misses = 0;
    //setting size of sets
    cache->sets = (struct Sets_ *) malloc (setCount * sizeof(struct Sets_));

    //loop to sets the size of the blocks inside each set
    for (int i = 0; i < setCount; i++) {
        //setting the size of the blocks
        cache->sets[i].blocks = (struct Block_ *) malloc (associativity * sizeof(struct Block_));
        for (int j = 0; j < associativity; j++) {
            //initializing each value in the block to 0
            cache->sets[i].blocks[j].tag = 0;
            cache->sets[i].blocks[j].offset = 0;
            cache->sets[i].blocks[j].accessed = 0;
        }
    }
    //initialized bits to 0
    int indexBit = 0;
    int offsetBit = 0;
    int tagBit = 0;
    //initialized addresses to 0
    int offsetAddress = 0;
    int indexAddress = 0;
    int tagAddress = 0;
    //initialized maskbits to 0
    int maskOffset = 0;
    int maskIndex = 0;
    //initialized addressCount (number of addresses) to 0
    long int addressCount = 0;
    //initialized the variables for deciding to add items
    //to cache or replace items in cache to 0
    int declareValues = 0;
    int replaceEntry = 0;



    // calculate tagBit, indexBit and offsetBit for 32 bit address
    //given block size, setCount,  num of blocks and associativity

    //fully associative
    if (associativity == number_of_cache_blocks) {
        offsetBit = log2(cache_block_size);

        tagBit = 32 - offsetBit;
        g_num_cache_tag_bits = tagBit;
        g_cache_offset_bits = offsetBit;

    }
    //m-set associative
    if (associativity > 1 && associativity < number_of_cache_blocks) {
        indexBit = log2(setCount);
        offsetBit = log2(cache_block_size);

        tagBit = 32 - indexBit - offsetBit;
        g_num_cache_tag_bits = tagBit;
        g_cache_offset_bits = offsetBit;

    }
    //direct mapped
    if (associativity == 1) {
        indexBit = log2(number_of_cache_blocks);
        offsetBit = log2(cache_block_size);

        tagBit = 32 - indexBit - offsetBit;
        g_num_cache_tag_bits = tagBit;
        g_cache_offset_bits = offsetBit;

    }


    //calculate bitmask to shift offset
    for (int i = 0; i < offsetBit; i++) {
        maskOffset |= (1 << i);
    }

    //calculate bitmask to shift for index
    for (int i = 0; i < indexBit; i++) {
        maskIndex |= (1 << i);
    }


    /* Loop until the whole trace file has been read. */
    while(1) {
        access = read_transaction(ptr_file);
        // If no transactions left, break out of loop.
        if (access.address == 0)
            break;
        /* Add your code here */

        //check for fully associative or not
        if (associativity == number_of_cache_blocks) {
            //calcuate address and offset when fully associative
            tagAddress = (access.address >> (offsetBit));
            offsetAddress = (access.address & maskOffset);
        } else {
            //calcuate address, offset and index when not fully associative
            tagAddress = (access.address >> (offsetBit + indexBit));
            offsetAddress = (access.address & maskOffset);
            indexAddress = ((access.address >> offsetBit) & maskIndex);
        }



        //loop for each address entered to see if the conditions are met
        for (int i = 0; i < associativity ; i++) {
            //check if the tagAddress matches a tag already in the cache
            if (tagAddress == cache->sets[indexAddress].blocks[i].tag) {
                cache->cache_hits += 1;
                cache->sets[indexAddress].blocks[i].accessed = addressCount;
                cache->sets[indexAddress].blocks[i].offset = offsetAddress;
                declareValues = 0;
                replaceEntry = 0;
                break;
                //if it does not match, prepare to to add the entry or replace by
                //the specified replacement policy
            } else if (tagAddress != cache->sets[indexAddress].blocks[i].tag){
                declareValues = 1;
                replaceEntry = 1;
            }
        }
        if (declareValues == 1) {
            declareValues = 0;
            //find the first empty position in the current set and replace with new entry
            for (int j = 0; j < associativity; j++) {
                if (cache->sets[indexAddress].blocks[j].tag == 0) {
                    replaceEntry = 0;
                    cache->sets[indexAddress].blocks[j].tag = tagAddress;
                    cache->sets[indexAddress].blocks[j].offset = offsetAddress;
                    cache->sets[indexAddress].blocks[j].accessed = addressCount;
                    break;
                }
            }
        }
        if (replaceEntry == 1) {
            replaceEntry = 0;
            cache->cache_misses += 1;
            //replacing by LRU
            if (replacement_policy == LRU) {
                int findingLowestAccess = cache->sets[indexAddress].blocks[0].accessed;
                int tempLowestAccess = 1;
                int indexAtLowest = 0;
                //find the index that has the lowest accessed value (accessed in the longest time)
                for (int j =0; j < associativity; j++) {
                    tempLowestAccess = cache->sets[indexAddress].blocks[j].accessed;
                    if (tempLowestAccess < findingLowestAccess) {
                        findingLowestAccess = tempLowestAccess;
                        indexAtLowest = j;
                    }
                }
                cache->sets[indexAddress].blocks[indexAtLowest].tag = tagAddress;
                cache->sets[indexAddress].blocks[indexAtLowest].offset = offsetAddress;
                cache->sets[indexAddress].blocks[indexAtLowest].accessed = addressCount;
                //replacing by FIFO
            } else if (replacement_policy == FIFO) {
                cache->sets[indexAddress].blocks[0].tag = tagAddress;
                cache->sets[indexAddress].blocks[0].offset = offsetAddress;
                cache->sets[indexAddress].blocks[0].accessed = addressCount;
                //replacing by Random
            } else if (replacement_policy == Random) {
                int randomNum = rand() % associativity;
                cache->sets[indexAddress].blocks[randomNum].tag = tagAddress;
                cache->sets[indexAddress].blocks[randomNum].offset = offsetAddress;
                cache->sets[indexAddress].blocks[randomNum].accessed = addressCount;
            }
        }
        //increment the addressCount for each new address being read
        //from file
        addressCount++;
    }

    g_result.cache_hits = cache->cache_hits;
    g_result.cache_misses = cache->cache_misses;


    //freeing stored memory

    for (int i = 0; i < setCount; i++) {
        free(cache->sets[i].blocks);
    }
    free(cache->sets);
    free(cache);


    /* Do not modify code below. */
    /* Make sure that all the parameters are appropriately populated. */
    print_statistics(g_num_cache_tag_bits, g_cache_offset_bits, &g_result);

    /* Close the trace file. */
    fclose(ptr_file);
    return 0;
}
