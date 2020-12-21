/**
 * This program reads a text file and tracks the frequency of all words in the file. It then outputs 
 * the words in order of descending frequency into another text file. Memory is dynamically allocated 
 * when storing each new word encountered. The file name to be read is provided using command line 
 * arguments. If the file name is not provided or the file is not readable, the program will exit
 * and provide an error message.
 *
 * @author Bryan Bennett {@literal <bennbc16@wfu.edu>}
 * @date Oct. 5, 2020
 * @assignment Lab 3
 * @course CSC 250
 **/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*Struct Definitions*/
struct WordFreq
{
    char *word; /* the word */
    int count;  /* the word frequency */
};


/*Function Prototypes*/
void wordArray(struct WordFreq **currArrayPtr, char *myStringPtr, int *numWordsPtr, int numLetters);
void sortArray(struct WordFreq *currArrayPtr, int numWords);
void printAndFree(struct WordFreq *currArrayPtr, int numWordsPtr, FILE *fptr);
void dynamicString(char **list, char c, int numLetters);


/*Function Definitions*/
int main(int argc, char *argv[])
{
    char fileWriteName[25] = {"\0"};
    strncpy(fileWriteName, argv[1], strlen(argv[1])+1);
    strncat(fileWriteName, ".frq", 5);

    /* Variable Declarations */
    FILE *fptrR = fopen(argv[1], "re");                                     /*Open READ file */
    FILE *fptrW = fopen(fileWriteName, "we");                                  /*Creates and opens a WRITE file */
    
    int numWords = 0;                                                       /*Tracks number of UNIQUE words*/
    int numLetters = 0;                                                     /*Number of letters in static array*/
    
    struct WordFreq *currArrayPtr = NULL;                                   /*This is the array used to track all word structs*/
    char *myString = NULL;                                                      /*Static string used in main to create words*/
    char c;                                                                 /*Stores each char in the read file*/
    

    /* File errors and corresponding messages */
    if (argc < 2){                                                          /*Input file name not sent via command line*/
        printf("Usage: %s filename \n", argv[0]);
        return 1;
    } 
    if (fptrR == NULL){                                                     /*Input file DNE or cannot be read*/
        printf("File does not exist or cannot be read\n");
        return 1;
    } 

    /* Navigates through input file, chunks chars into words and sends them to wordArray() for parsing */
    c = fgetc(fptrR);
    while (!feof(fptrR)){

        if (isalpha(c)){                                                    /*If char is letter, make lowercase and add to char array*/
            c = tolower(c);
            dynamicString(&myString, c, numLetters);
            numLetters++;
        }
        else if (numLetters != 0){                                          /*If char is not letter and we have non-empty string, send word to wordArray*/
            wordArray(&currArrayPtr, myString, &numWords, numLetters);
            numLetters = 0;
        }

        c = fgetc(fptrR);                                                   /*Get next char*/
    }

    sortArray(currArrayPtr, numWords);                                      /*Sorts array in ascending order*/
    printAndFree(currArrayPtr, numWords, fptrW);                            /*Prints array in descending order (greatest first), then frees all memory*/
    free(myString);

    fclose(fptrR);
    fclose(fptrW);

    return 0;
}


/* wordArray interacts with currArray to track all words and their frequencies. Novel words (passed as strings) dynamically grow currArray*/
void wordArray(struct WordFreq **currArrayPtr, char *myStringPtr, int *numWordsPtr, int numLetters)
{
    int i = 0;
    int isMatch = 0;

    /*If myString is in a struct in the current array, increment the count*/
    while (i < *numWordsPtr && isMatch == 0){
        if (strcmp(myStringPtr, (*currArrayPtr)[i].word) == 0){
            (*currArrayPtr)[i].count = (*currArrayPtr)[i].count + 1;
            isMatch = 1;
        }
        i++;
    }

    /*If string does not match any existing words, create larger array and add word*/
    if (isMatch == 0){

        struct WordFreq *newArrayPtr = (struct WordFreq *)malloc(sizeof(struct WordFreq) * (*numWordsPtr + 1));     /*Dynamically create larger array of structs*/
        
        if (*currArrayPtr){
            memcpy(newArrayPtr, *currArrayPtr, (sizeof(struct WordFreq)) * (*numWordsPtr));                         /*Copy old array over*/
            free(*currArrayPtr);                          
        }

        newArrayPtr[*numWordsPtr].word = (char *)malloc(sizeof(char) * (numLetters + 1));                           /*Allocate space for new word*/
        strncpy(newArrayPtr[*numWordsPtr].word, myStringPtr, (numLetters + 1));                                     /*Copy word into new location within struct*/
        
        newArrayPtr[*numWordsPtr].count = 1;                                                                        /*Set count equal to 1*/
        *numWordsPtr = *numWordsPtr + 1;                                                                            /*Increment numWords*/

        *currArrayPtr = newArrayPtr;                                                                                 /*Point currArray to newArray*/
        fflush(stdout);
    }

}


/*Utilizes interstion sort to manipulates currArray into ASCENDING order (most frequent word at the last index)*/
void sortArray(struct WordFreq *currArrayPtr, int numWords)
{
    int i;
    int j;
    struct WordFreq tmp;                                                        /*Temporarily stores a struct. For use in loop*/

    /*Insertion sort algo. Navigates currArray and performs swaps*/
    for (i = 1; i < numWords; i++){
        for (j = i; j > 0; j--){
            if (currArrayPtr[j - 1].count > currArrayPtr[j].count){             /*If value @x-1 > value @x, move value @x left*/
                tmp = currArrayPtr[j];
                currArrayPtr[j] = currArrayPtr[j - 1];
                currArrayPtr[j - 1] = tmp;
            }
        }
    }
}


/*Prints array from last index position to first, freeing each char* along the way and then freeing currArray*/
void printAndFree(struct WordFreq *currArrayPtr, int numWords, FILE *fptr)
{
    int i = numWords - 1;

    while (i > -1){
        fprintf(fptr, "%s %d\n", currArrayPtr[i].word, currArrayPtr[i].count);      /*Print word followed by frequency*/
        free(currArrayPtr[i].word);                                                 /*Free previously allocated space for word*/
        i--;
    }

    free(currArrayPtr);                                                             /*Free currArray. No memory leaks*/

}

void dynamicString(char **list, char c, int numLetters)
{
    char *tmpList = (char *) malloc(sizeof(char) * (numLetters + 2));    

    if (*list){
        memcpy(tmpList, *list, (sizeof(char)) * (numLetters));                         /*Copy old array over*/
        free(*list);                          
    }

    tmpList[numLetters] = c;
    tmpList[numLetters + 1] = '\0';                           
    *list = tmpList;                             
}