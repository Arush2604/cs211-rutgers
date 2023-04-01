#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Malloc stuff for temp variables
int tempcount = 0;
int outputsactive = 0;


typedef struct vars {
    int val;
    char name[200];
} VARS;

typedef struct circuit {
    int inputsize;
    int outputsize;
    VARS* inputs;
    VARS* outputs;
    VARS* temp;
} circuit;

void MULTI(circuit fullcirc, FILE * fp);
void DECODER(circuit fullcirc, FILE * fp);

//find for existing Input variables
int find(circuit circ, char * name) {
    for(int i = 0; i < circ.inputsize; i++) {
        if(strcmp(circ.inputs[i].name, name) == 0) {
            //printf("Val of %s is %d\n", name, circ.inputs[i].val);
            return i;
        }
        
    }
    return -1; 
}

//find for existing output variables
int findout(circuit circ, char * name) {
    if(outputsactive == 0) {
        return -1;
    }

    for(int i = 0; i < circ.outputsize; i++) {
        if(strcmp(circ.outputs[i].name, name) == 0) {
            //printf("Val of %s is %d\n", name, circ.outputs[i].val);
            return i;
        }
    }
    return -1; 
}

void maketemp(circuit circ, char * name, int value) {
   
    strcpy(circ.temp[tempcount].name, name);
    circ.temp[tempcount].val = value;
    tempcount++;

}

//find for temp variables
int findtemp(circuit circ, char * name) {
    if(tempcount == 0){
        return -1;
    }
    for(int i = 0; i < tempcount; i++) {
        if(strcmp(circ.temp[i].name, name) == 0) {
            //printf("Val of %s is %d\n", name, circ.outputs[i].val);
            return i;
        }
    }

    return -1; 
}

void AND(circuit fullcirc, FILE * fp) {
    char inputvar[50];
    char outputvar[50];
    int* inputs;
    inputs = malloc(2*sizeof(int));

    //inputs 
    for(int j = 0; j<2; j++) {
        fscanf(fp, "%s", inputvar);
        int index = find(fullcirc, inputvar);
        if(index != -1) {
            inputs[j] = fullcirc.inputs[index].val;
        }
        else if(strcmp(inputvar, "1") == 0 || strcmp(inputvar, "0") == 0) {
            inputs[j] =  atoi(inputvar);
        }
        else {
            int tempindex = findtemp(fullcirc, inputvar);
            inputs[j] = fullcirc.temp[tempindex].val;

        }
    }
    //outputs
    fscanf(fp, "%s", outputvar);
    int res = inputs[0] && inputs[1];
    int outindex = findout(fullcirc, outputvar);
    if(outindex != -1) {
        fullcirc.outputs[outindex].val = res;
    }
    else {
        maketemp(fullcirc, outputvar, res);
        //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));
    }
    free(inputs);
}

void OR(circuit fullcirc, FILE * fp) {
    char inputvar[50];
    char outputvar[50];
    int* inputs;
    inputs = malloc(2*sizeof(int));

    //inputs 
    for(int j = 0; j<2; j++) {
        fscanf(fp, "%s", inputvar);
        int index = find(fullcirc, inputvar);
        if(index != -1) {
            inputs[j] = fullcirc.inputs[index].val;
        }
        else if(strcmp(inputvar, "1") == 0 || strcmp(inputvar, "0") == 0) {
            inputs[j] =  atoi(inputvar);
        }
        else {
            int tempindex = findtemp(fullcirc, inputvar);
            inputs[j] = fullcirc.temp[tempindex].val;

        }
    }
    //outputs
    fscanf(fp, "%s", outputvar);
    int res = inputs[0] || inputs[1];
    int outindex = findout(fullcirc, outputvar);
    if(outindex != -1) {
        fullcirc.outputs[outindex].val = res;
    }
    
    else {
        maketemp(fullcirc, outputvar, res);
        //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));
    }
    free(inputs);
}

void XOR(circuit fullcirc, FILE * fp) {
    char inputvar[50];
    char outputvar[50];
    int* inputs;
    inputs = malloc(2*sizeof(int));

    //inputs 
    for(int j = 0; j<2; j++) {
        fscanf(fp, "%s", inputvar);
        int index = find(fullcirc, inputvar);
        if(index != -1) {
            inputs[j] = fullcirc.inputs[index].val;
        }
        else if(strcmp(inputvar, "1") == 0 || strcmp(inputvar, "0") == 0) {
            inputs[j] =  atoi(inputvar);
        }
        else {
            int tempindex = findtemp(fullcirc, inputvar);
            inputs[j] = fullcirc.temp[tempindex].val;

        }
    }
    //outputs
    fscanf(fp, "%s", outputvar);
    int res = inputs[0] ^ inputs[1];
    int outindex = findout(fullcirc, outputvar);
    if(outindex != -1) {
        fullcirc.outputs[outindex].val = res;
    }
    else {
        maketemp(fullcirc, outputvar, res);
        //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));
    }
    free(inputs);
}

void NOT(circuit fullcirc, FILE * fp) {
    char inputvar[50];
    char outputvar[50];
    int ninputvar;

    //inputs 
    fscanf(fp, "%s", inputvar);
    int index = find(fullcirc, inputvar);
    if(index != -1) {
        ninputvar = fullcirc.inputs[index].val;
    }
    else if(strcmp(inputvar, "1") == 0 || strcmp(inputvar, "0") == 0) {
        ninputvar =  atoi(inputvar);
    }
    else {
        int tempindex = findtemp(fullcirc, inputvar);
        ninputvar = fullcirc.temp[tempindex].val;
    }
    
    //outputs
    fscanf(fp, "%s", outputvar);
    int res = !ninputvar;
    int outindex = findout(fullcirc, outputvar);
    if(outindex != -1) {
        fullcirc.outputs[outindex].val = res;
    }
    else {
        maketemp(fullcirc, outputvar, res);
        //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));
    }
}

void MULTI(circuit fullcirc, FILE * fp) {

    char inputvar[200];
    int* multiplexerinputs;

    char selector[200];
    int* selectors;

    int nummultiplexerinputs;

    fscanf(fp, "%d", &nummultiplexerinputs);
    //printf("%d", nummultiplexerinputs);
    int len = log2(nummultiplexerinputs);
    multiplexerinputs = malloc(nummultiplexerinputs * sizeof(int));

 
    for(int i = 0; i < nummultiplexerinputs; i++) {
        fscanf(fp, "%s", inputvar);
        int index = find(fullcirc, inputvar);
        if(index != -1) {
            multiplexerinputs[i] = fullcirc.inputs[index].val;
        }
        else if(strcmp(inputvar, "1") == 0 || strcmp(inputvar, "0") == 0) {
            multiplexerinputs[i] =  atoi(inputvar);
        }
        else {
            int tempindex = findtemp(fullcirc, inputvar);
            multiplexerinputs[i] = fullcirc.temp[tempindex].val;
        }
    }

    
    selectors = malloc(len * sizeof(int));
    char* ptr;
    for(int j = 0; j<len; j++) {
        fscanf(fp, "%s", selector);
        int selindexout = findout(fullcirc, selector);
        if(strcmp(selector, "1") == 0 || strcmp(selector, "0") == 0) {
            selectors[j] =  strtol(selector, &ptr, 10);
        }
        else if(selindexout == -1) {
            int selindexin = find(fullcirc, selector);
            if(selindexin == -1) {
                int selindextemp = findtemp(fullcirc, selector);
                selectors[j] = fullcirc.temp[selindextemp].val;
            }
            else{
                selectors[j] = fullcirc.inputs[selindexin].val;
            }
        }
        else{
            selectors[j] = fullcirc.outputs[selindexout].val;
        }

    }

    int filler = 0;
    int final = 0;

    for(int k = 0; k<len; k++) {
        final += selectors[k] * pow(2, (len-(k+1)));
    }

    char outputvar[200];
    fscanf(fp, "%s", outputvar);
    int outindex = findout(fullcirc, outputvar);
    if(outindex != -1) {
        for(int t = 0; t<nummultiplexerinputs; t++) {
            if((t ^ (t>>1)) == final) {
                fullcirc.outputs[outindex].val = multiplexerinputs[t];
            }
        }
    }
    else {
        maketemp(fullcirc, outputvar, filler);
        int tempindex2 = findtemp(fullcirc, outputvar);
        for(int t = 0; t<nummultiplexerinputs; t++) {
            if((t ^ (t>>1)) == final){
                fullcirc.temp[tempindex2].val = multiplexerinputs[t];
            }
        }
    }

    free(multiplexerinputs);
    free(selectors);

}

void DECODER(circuit fullcirc, FILE * fp) {
    int amtdcdinputs;
    int* dcdinputs;
    int* dcdoutputs;
    int final = 0;

    char inputvar[200];
    char outputvar[200];

    fscanf(fp, "%d", &amtdcdinputs);
    dcdinputs = malloc(amtdcdinputs * sizeof(int));

    int amtdcdoutputs = pow(2, amtdcdinputs);
    dcdoutputs = malloc(amtdcdoutputs * sizeof(int));

    for(int i = 0; i < amtdcdinputs; i++) {
        fscanf(fp, "%s", inputvar);
        int index = find(fullcirc, inputvar);
        if(index != -1) {
            dcdinputs[i] = fullcirc.inputs[index].val;
        }
        else if(strcmp(inputvar, "1") == 0 || strcmp(inputvar, "0") == 0) {
            dcdinputs[i] =  atoi(inputvar);
        }
        else {
            int tempindex = findtemp(fullcirc, inputvar);
            dcdinputs[i] = fullcirc.temp[tempindex].val;
        }
    }

    for(int k = 0; k<amtdcdinputs; k++) {
        final += dcdinputs[k] * pow(2, (amtdcdinputs-(k+1)));
    }

    int res;
    for(int p = 0; p<amtdcdoutputs;p++) {
        fscanf(fp, "%s", outputvar);
        int outindex = findout(fullcirc, outputvar);
        if(outindex != -1 && (p ^ (p>>1)) != final) {
            fullcirc.outputs[outindex].val = 0;
        }
        else if(outindex != -1 && (p ^ (p>>1)) == final) {
            fullcirc.outputs[outindex].val = 1;
        }
        else if(outindex == -1 && (p ^ (p>>1)) != final) {
            res = 0;
            maketemp(fullcirc, outputvar, res);
        }
        else if(outindex == -1 && (p ^ (p>>1)) == final) {
            res = 1;
            maketemp(fullcirc, outputvar, res);
        }


    }

    free(dcdinputs);
    free(dcdoutputs);


}

int main(int argc, char * argv[argc+1])
{
    FILE * fphead = fopen(argv[1], "r");
    FILE * fphead2 = fopen(argv[1], "r");

    //store number of directions
    int directioncount; 
    for(int i  = 0; i < 1; i++){
        fscanf(fphead, "%d", &directioncount);
    }
    //printf("%d\n", directioncount);

   //retrieve input values
   int skiplines = directioncount + 1;
   char line[250];
   int count = 0;
   while(fgets(line, sizeof(line), fphead)) {
    if (count == skiplines) {
        //printf("%s", line);
        //simulate(line, argv[1]);
        skiplines++;
    }
    count++;
   }
   int fileinputs = (count - directioncount - 2);
   //printf("%d different inputs\n", fileinputs);
   int* inputstouse = malloc(fileinputs * sizeof(int));

    char line2[250];
    int count2 = 0;
    int skiplines2 = directioncount + 1;
    int iminpain = 0;
    char* ptr;
    while(fgets(line2, sizeof(line2), fphead2) && iminpain < fileinputs) {
    if (count2 == skiplines2) {
        inputstouse[iminpain] = strtol(line2, &ptr, 10);
        skiplines2++;
        iminpain++;
    }
    count2++;
   }

   for(int h = 0; h < fileinputs; h++) {
    circuit fullcirc;
    int x = inputstouse[h];
    FILE * fp = fopen(argv[1], "r");
    char command[20];
    fullcirc.temp = malloc(150 * sizeof(VARS));
    
    while(fscanf(fp, "%s", command) != EOF) {

        if(strcmp(command, "INPUTVAR") == 0) {
            char iname[200];
            fscanf(fp, "%d", &fullcirc.inputsize);
            //printf("%d\n", fullcirc.inputsize);
            fullcirc.inputs = malloc(fullcirc.inputsize * sizeof(VARS));
            for(int i = 0; i < fullcirc.inputsize; i++) {
                fscanf(fp, "%s", iname);
                strcpy(fullcirc.inputs[i].name, iname);
                //printf("%s\n", fullcirc.inputs[i].name);
                fullcirc.inputs[i].val =  ((x >> (fullcirc.inputsize-(i+1))) & 1);
                //printf("Variable %s has value: %d\n",fullcirc.inputs[i].name, fullcirc.inputs[i].val);     
            }
        }
        else if(strcmp(command, "OUTPUTVAR") == 0) {
            outputsactive = 1;
            char oname[200];
            fscanf(fp, "%d", &fullcirc.outputsize);
            //printf("%d\n", fullcirc.outputsize);
           
            fullcirc.outputs = malloc(fullcirc.outputsize * sizeof(VARS));
            for(int i = 0; i < fullcirc.outputsize; i++) {
                fscanf(fp, "%s", oname);
                //fullcirc.outputs[i].name = oname;
                strcpy(fullcirc.outputs[i].name, oname);
                fullcirc.outputs[i].val = 0;
                //printf("Variable %s has value: %d\n",oname, fullcirc.outputs[i].val);
                
            }
        }
        else if(strcmp(command, "AND") == 0) {
            AND(fullcirc, fp);
            //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));
        }

        else if(strcmp(command, "OR") == 0) {
            OR(fullcirc, fp);
            //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));     
        }

        else if(strcmp(command, "XOR") == 0) {
            XOR(fullcirc, fp);
            //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));    
        }

        else if(strcmp(command, "NOT") == 0) {
            NOT(fullcirc, fp);
            //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));     
        }

        else if(strcmp(command, "DECODER") == 0) {
            DECODER(fullcirc, fp);
            //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));   
        }

        else if(strcmp(command, "MULTIPLEXER") == 0) {
            MULTI(fullcirc, fp);
            //fullcirc.temp = realloc(fullcirc.temp, (tempcount+1)*sizeof(VARS));
        }
    }

    //print values
    for(int i = 0; i < fullcirc.outputsize; i++) {
        if(fullcirc.outputsize == 1) {
            printf("%d", fullcirc.outputs[i].val);
        }
        else{
            printf("%d\t", fullcirc.outputs[i].val);
        }
    }
    printf("\n");

    
    free(fullcirc.outputs);
    free(fullcirc.inputs);
    free(fullcirc.temp);
    tempcount = 0;
    outputsactive = 0;
    
   }

   free(inputstouse);
   fclose(fphead);
   fclose(fphead2);


   return EXIT_SUCCESS;
}

