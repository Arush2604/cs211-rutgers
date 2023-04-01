
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int misses = 0;
int hits = 0;
int reads = 0;
int writes = 0;

int age;
int counter;

struct Node** cache;

int sets;
int size;
int assoc;
int blocks;

typedef struct Node {
    int isvalid; 
    int age;
    unsigned long tag;
    
} Node;

struct Node** make(int sets, int assoc) {
    cache = (Node**)malloc(sizeof(Node*)*sets);
    for(int i = 0; i < sets; i++) {
        cache[i] = (Node*)malloc(sizeof(Node)*assoc);
    }

    for(int j = 0; j < sets; j++){
        for(int k = 0; k < assoc; k++) {
            cache[j][k].isvalid = 0;

        }
    }

    return cache;
}

//check if number is a power of 2 for input
int ispw2(int x) { //0 means no, 1 means it is
    if(x == 0) {
        return 0;
    }
    while(x != 1) {
        if(x % 2 != 0) {
            return 0;
        }
        x = x/2;
    }
    return 1;
}

//actual method
void toread( int assoc, unsigned long tag_index, unsigned long set_index) { 
    int end = assoc-1;

    for(int i=0; i<assoc;i++) {
        if(cache[set_index][i].isvalid == 1) { //space is full, count as hit
            if(cache[set_index][i].tag==tag_index) {
                hits++; 
                return;
            }    
        }

        else { //empty space! Insert data
            age++;
            reads++;
            misses++;

            cache[set_index][i].isvalid = 1;
            cache[set_index][i].tag = tag_index;
            cache[set_index][i].age = age;
            return;
        }
    
        if(i==end){ //if weve reached the end of the loop and value is still not inserted, evict using value with lowest age (fifo)
            reads++;
            misses++;

            int toevict = 0;
            int counter = 0;

            while(counter < assoc) {
                if(cache[set_index][counter].age <= cache[set_index][toevict].age) {
                    toevict = counter;
                }
                counter++;
            }

            age++;
            cache[set_index][toevict].isvalid=1;
            cache[set_index][toevict].tag = tag_index;
            cache[set_index][toevict].age = age;
            return;
        }
    }
    return;
}


//write
void towrite(unsigned long long int tag_index, unsigned long long int set_index, int assoc) { 
    toread(tag_index,set_index,assoc);
    writes++;
    return;
}

//error checking input
int error(int argc, char ** input) {
    //error checking
    if(argc != 5) {
        printf("ERROR\n");
    }

    //size
    size = atoi(input[1]);
    if(ispw2(size) == 0) {
        printf("ERROR\n");
    }

    //blocks
     blocks = atoi(input[3]);
     if(ispw2(blocks) == 0) {
        printf("ERROR\n");
        return 0;
    }

    //assoc
    if(strcmp(input[2], "direct")==0){
		assoc = 1;
		sets = size/blocks;
	}
    else if(strcmp(input[2], "assoc")==0){
		sets = 1;
        assoc = size/blocks;
	}
    else if(strncmp(input[2], "assoc:", 6)==0){
		sscanf(input[2], "assoc:%d", &assoc);
		if(ispw2(assoc)==0){
			printf("ERROR\n");
			return 0;
		}
		sets = size/(assoc*blocks);
	}
    else{
		printf("ERROR\n");
		return 0;
	}
    return 0;

}

int main(int argc, char * argv[argc+1])
{

    char ** input;
    input = argv;
    error(argc, input);
    

    FILE * fp = fopen(argv[4],"r");
    if(fp ==NULL){
        printf("ERROR");
        return 0;
    }

    cache = make(sets, assoc);
    int bit_blocks = log2(blocks);
    int bit_sets = log2(sets);

    int mask = ((1 << bit_sets) - 1);
    unsigned long set_index;
    unsigned long tag_index;

    char operation;
    unsigned long address;
    
    while(fscanf(fp, "%c %lx\n", &operation, &address)!=EOF){
        set_index=(address>>bit_blocks) & mask;
        tag_index=(address>>bit_blocks) >> bit_sets;

        if(operation =='R'){
            toread(assoc,tag_index,set_index);
            continue;
        }
        else if(operation =='W'){
            towrite(assoc, tag_index,set_index);
            continue;
        }
        else{
            printf("ERROR\n");
            break;
        }
    }

    //free cache
    for(int i=0;i<sets;i++){
        free(cache[i]);
    }
    free(cache);

    printf("memread:%d\n", reads);
    printf("memwrite:%d\n", writes);
    printf("cachehit:%d\n", hits);
    printf("cachemiss:%d\n", misses);
 

    return EXIT_SUCCESS;
}




