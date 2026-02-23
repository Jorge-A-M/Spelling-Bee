/*-------------------------------------------
Project 3: Spelling Bee
        The following program is inspired by the New York
        Times Spelling Bee minigame. 
        
        The program has two primary functionalities being a game 
        and a solver which are modes toggled through the 
        command line argument -p.

        This particular implementation deviates from the standard
        minigame of only finding words from a given list with a 
        specified letter by permitting user customizability of the
        games through various different flags.

        The gameplay itself introduces a new way to play with the
        addition of user-selected, concrete point goals, a time-based
        point system, and a local leaderboard.

        Before execution end, the program displays several statistics
        about the hive and all its valid solutions including a complete list of
        such.
Author: Jorge Martin
Last Modified On: 10/21/2025
------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

const int MIN_HIVE_SIZE = 2;
const int MAX_HIVE_SIZE = 12;
const int MIN_WORD_LENGTH = 4;

typedef struct WordList_struct {
    char** words; //array of pointers to words for the list, (the list of words itself)
    int numWords; //current number of words in the word array
    int capacity; //maximum number of words current version of the list can hold (may be expanded during runtime)
} WordList;

//Creates a WordList struct object with default values (capacity of 4, no words) and allocates the appropriate memory in the heap
//No parameters
//returns a WordList object, a list initilized with default values
WordList* createWordList() {
    WordList* newList = malloc(sizeof(WordList)); //allocates the object in the heap
    newList->capacity = 4;
    newList->numWords = 0;
    newList->words = malloc(newList->capacity * sizeof(char*)); //allocates the list of pointers to words itself in the heap

    return newList;
}

/*
Adds the passed word to the end of the selected word list. Dynamically resizes the list if it has reached maximum capacity

thisWordList is a WordList object containing an array of pointers to which newWord's allocation address will be added to
newWord is the character array added to the list of words. It's contents are stored in the heap and its pointer is saved in passed word list

No return value, updates the passed WordList object accordingly (adds word address and increases size if needed)
*/
void appendWord(WordList* thisWordList, char* newWord) {
    char** tempList = NULL; //pointer array used to allocate required space for the word list
    char* newWordMallocd = NULL;
    
    if (thisWordList->numWords == thisWordList->capacity) {
        tempList = (char**)malloc(sizeof(char*) * (thisWordList->capacity * 2)); //capacity is doubled after being maximized

        //elements from word list's pointer array are copied over from the smaller to the new, larger pointer array
        for (int i = 0; i < thisWordList->numWords; i++) {
            tempList[i] = thisWordList->words[i];
        }

        free(thisWordList->words); //memory for the smaller, now copied, array is freed
        thisWordList->words = tempList;

        thisWordList->capacity *= 2; //update the list's variable keeping track of capacity
    }
    
    newWordMallocd = (char*)malloc(strlen(newWord) + 1); //allocates enough space in the heap for the size of the word + '\0'
    strcpy(newWordMallocd, newWord); //stores char array in the heap
    thisWordList->words[thisWordList->numWords] = newWordMallocd; //appends the string to the word list array 

    (thisWordList->numWords)++;
}

/*
Sets up the list that will serve as the game's dictionary. Takes in file input to populate a WordList object with words of a specified length

filename is the string referencing the file used to populate the list with words
dictionaryList is the WordList object to be populated and serve as the game's dictionary of potentially valid words
minLength is an int specifiying the minimum length of each word that is added to the array. Smaller words than minLength are not appended

returns an int, the length of the longest valid word found while populating the array or -1 if the operation is unsuccessful
*/
int buildDictionary(char* filename, WordList* dictionaryList, int minLength) {
    int dictIndex = 0;
    int longestWordLength = 0; 
    char currentWord[50];

    FILE* dictFilePtr = fopen(filename, "r"); //file is opened to be read from/be used as input
    if (dictFilePtr == NULL) { //terminate program if opening the file fails
        return -1; 
    }

    //read in from the file until the end is reached
    while (!feof(dictFilePtr)) {
        fscanf(dictFilePtr, "%49s", currentWord);

        //append word to the list if it meets length requirement
        if (strlen(currentWord) >= minLength) {
            appendWord(dictionaryList, currentWord);

            //compare string lengths everytime a valid word is found to find the longest string appended
            if (strlen(dictionaryList->words[dictIndex]) > longestWordLength) {
                longestWordLength = strlen(dictionaryList->words[dictIndex]);
            }
            
            dictIndex++; 
        }    
    }

    fclose(dictFilePtr);
    
    return longestWordLength;
}

/*
Frees the heap-allocated memory used by a particular wordList including the character arrays and 
the array of character arrays each holds as well the list itself

list is the WorldList to be completely freed

no return value, frees all memory allocated for and within the passed list
*/
void freeWordList(WordList* list) {
    //iterates through every element in the WordList's pointer array to free the memory allocated for the individual char arrays
    for (int i = 0; i < list->numWords; i++) {
        free(list->words[i]);
    }

    //frees the list's array of pointers as well as the list itself
    free(list->words);
    free(list);
}

/*
Searches a string for a specific, the passed, letter (aLet)

str is the character array where the specified letter will be looked for
aLet is the character/letter being looked for inside str

returns an int, the index of the first occurrence of the letter in relation to where it is found in the word
otherwise returns -1 if the letter is not found
*/
int findLetter(char* str, char aLet) {
    //compares every character in the string with the passed letter in search of any matches 
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == aLet) {
            return i;
        }
    }

    return -1; //letter not found within string
}

/*
Constructs a hive, the string containing the game's valid letters, from user input. 
Alphabetizes the string.

str is the passed string that will be alphabetizes and stored inside hive

hive is the string that will recieve the alphabetized word and be referenced for any gameplay/solver features
as the criterion for valid words

no return value, updates the hive variable which is passed as a pointer
*/
void buildHive(char* str, char* hive) {
    int letterIndex;
    int hiveIndices[strlen(str)];
    int hiveIndex = 0;
    char tempStr[strlen(str)];

    //iterates through the alphabet in order (from a - z) adding any letters found in the passed str to an array
    //since the loop iterates in alphabetical order, the character array is also organized alphabetically
    for (char currLetter = 'a'; currLetter <= 'z'; currLetter++) {
        letterIndex = findLetter(str, currLetter); //search for current iteration letter within the passed string

        //letter's index added to the array keeping track of the found characters if the letter is in the string
        if (letterIndex >= 0) {
            hiveIndices[hiveIndex] = letterIndex;
            hiveIndex++;
        }
    }
    
    //Constructs a string from the saved indexes organized in alphabetical order, the finalized hive
    for (int i = 0; i < hiveIndex; i++) {
        tempStr[i] = str[hiveIndices[i]];
        if (i == hiveIndex - 1) {
            tempStr[i+1] = '\0'; //null-terminates the string after the last letter added
        }
    }

    strcpy(hive, tempStr); //copies local copy/version of constructed hive onto the hive pointer
}

/*
Counts the number of different letters inside a specified string

str is the character array which will be analyzed for its unique letter count

returns an int, the number of different letters in the string
Value returned could be as large as the string's length if every character is different
*/
int countUniqueLetters(char* str) {
    int wordLen = strlen(str);
    int uniqueCount = 0;

    char uniqueLetArr[wordLen + 1]; //will store the unique characters found from the passed string

    //initializes every value in the array with a null character for safety (ensure string is null-terminated)
    for (int i = 0; i < wordLen; i++) {
        uniqueLetArr[i] = '\0';
    }
    
    for (int i = 0; i < wordLen; i++) {
        for (int j = 0; j < wordLen; j++) {
            if (findLetter(uniqueLetArr, str[j]) == -1) {
                uniqueLetArr[uniqueCount] = str[j];
                uniqueCount++; //increments count inside the loop checking if the letter had not previously shown up, hence unique
                uniqueLetArr[uniqueCount] = '\0'; //explicitely null-terminates the string after the last character added to ensure expected program functionality 
            }
        }
    }

    return uniqueCount;
}

/*
Finds all words of a specified length, being the number of unique letters equivalent to the hive size. 
In other words, the words that meet the criterion must have at the very least hiveSize unique letters in them
To become a hive, these letters will eventually be reduced to unique letters only 
(in the case that the length of the word > its unique letters)

dictionaryList is the list of words that have their unique letter count compared to hiveSize
hiveSize is the number of unique characters in the hive, the different characters used to form words

returns a WordList object, a list containing all of the words of hive size unique letters which will be used to build a hive
in random mode (random word from this list will serve as the base for the hive)
*/
WordList* findAllFitWords(WordList* dictionaryList, int hiveSize) {
    WordList* fitWords = createWordList(); //this list will be returned, will contain of hive size unique letters
    
    //check every word in the passed dictionary (list) and determine which have the same amount of unique characters as hive has/will have length
    for (int i = 0; i < dictionaryList->numWords; i++) {
        if (countUniqueLetters(dictionaryList->words[i]) == hiveSize) {
            appendWord(fitWords, dictionaryList->words[i]); //add the found word to list that will be returned
        }
    }

    return fitWords;
}

/*
Verify the validity of a passed word based on the criteria that the word contains only letters also found in the hive
and that it also contains the required letter from the hive

word is the string being evaluated on whether it uses only hive letters and includes the required letter
hive is string containing the letters that must be used in a word for it to be considered valid, including the required letter
reqLet is the character that must be found in word string for it to be a valid

returns a bool, true if the word contains both only letters also found in the hive and the required letter/character
false if either or both the criteria above are not met
*/
bool isValidWord(char* word, char* hive, char reqLet) {
    int wordLen = strlen(word);

    //checks if the checked string contains the required letter
    if (findLetter(word, reqLet) == -1) {
        return false; //required letter was not found in the string, invalid word
    }

    //checks every character in word comparing each character to those in the hive
    for (int i = 0; i < wordLen; i++) {
        if (findLetter(hive, word[i]) == -1) {
            return false; //word contains a letter that is not in the hive, invalid word
        }
    }

    return true;
}

/*
Checks whether the passed word/string is a pangram of the hive. 
A pangram is defined as a word that uses all of the letters from the given hive at least once.
i.e. "computer" is a pangram of "cemoprtu"

str is the word/character array checked for containing at least one of every letter in the hive
str is assumed to be confirmed as a valid word (contains only letters also found in the hive
and that it also contains the required letter from the hive)

hive is the string used to compared str to. str has to have all the letters in hive which are the valid play letters
including the required letter

returns a bool, true if str contains at least one of all letters in hive (may contain duplicates, but at least one of each)
false if str is missing even one letter also found in hive
*/
bool isPangram(char* str, char* hive) {
    //since str is assumed to be valid, the unique characters in it matching the number of characters
    //in hive indicates that all characters from hive were used (str cannot be composed of anything but letters also found in hive; hive contains no duplicates)
    if (countUniqueLetters(str) == strlen(hive)) {
        return true;
    }

    return false; //one or more characters were missing in str that are in hive
}

/*
Prints out the letters that can be used to create words and highlights the required letter with a '^'

hive is a string containing the letters available to make words from
reqLetInd is an integer indicating the letter that must be used in the word made for it to be valid

no retrun value, prints out the letters that can be used and marks the mandatory letter
*/
void printHive(char* hive, int reqLetInd) {
    printf("  Hive: \"%s\"\n", hive);
    printf("         ");
    for (int i = 0; i < reqLetInd; i++) { //adds spaces until required letter index
        printf(" "); 
    }
    printf("^"); //marks required letter
    for (int i = reqLetInd + 1; i < strlen(hive); i++) { //continues to add spaces below every non-required letter
        printf(" "); 
    }
    printf(" (all words must include \'%c\')\n\n", hive[reqLetInd]);
}

/*
Displays all words from a given list, particularly used for displaying the user's answers as they play

Provides additional information such as the points earned from the given word, total points, and whether the word is a pangram 
or even perfect pangram (pangram (*) contains all letters from hive at least once and perfect pangrams (***) containing every
letter from hive exactly once)

thisWordList is a wordList object, the list being displayed to the user

hive is the string containing the letters valid words can be formed from. Used to check whether the words in the passed
list are pangrams (all letters from hive are in the checked word)

no return value, displays the list alongside additional information such as points earned for said word, total points, and the word's status
as a pangram
*/
void printList(WordList* thisWordList, char* hive) {
    int hiveSize = strlen(hive);
    int currWordPoints = 0;
    int totScore = 0;

    printf("  Word List:\n");

    for (int wordIndex = 0; wordIndex < thisWordList->numWords; wordIndex++) { //iterates through every word in the list
        currWordPoints = 0;
        currWordPoints = strlen(thisWordList->words[wordIndex]); //all words, other than words of exactly minimum required length earn at least one point per letter

        //checks whether the word is a perfect pangram, all words from hive used exactly once
        if (isPangram(thisWordList->words[wordIndex], hive) && (hiveSize == strlen(thisWordList->words[wordIndex]))) {
            currWordPoints += hiveSize; //pangrams earn hiveSize extra points on top of their length points (for perfect pangrams these values are equal)
            totScore += currWordPoints; //word score added to total
            printf("*** (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]);
        }
        //checks whether the word is a pangram, all words from hive used at least once
        else if (isPangram(thisWordList->words[wordIndex], hive) && (hiveSize <= strlen(thisWordList->words[wordIndex]))) {
            currWordPoints += hiveSize; //pangrams earn hiveSize extra points on top of their length points
            totScore += currWordPoints; //word score added to total
            printf(" *  (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]);
        }
        //checks if the word is exactly the minimum required length
        else if (strlen(thisWordList->words[wordIndex]) == MIN_WORD_LENGTH){
            currWordPoints = 1; //only one total point is earned for the word if it is of exactly the minimum length
            totScore += currWordPoints; //word score added to total
            printf("    (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]);
        }
        //all other words
        else {
            totScore += currWordPoints; //word score added to total, all other words earn one point per letter
            printf("    (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]);
        }
    }
 
    printf("  Total Score: %d\n", totScore); //displays the sum of all points earned from individual words
}

/*
Iterates through every word through a WordList object, used particularly for the dictionary and 
determines whether the word in that iteration is a valid solution in the context of the game's hive

dictionaryList is the WordList object typically containing the list of all words of the specified minimum length
the words in this list are the ones to be determined as solutions or not

solveList is the list containing all words found to be solutions/valid words in the context of the hive
will be updated for use in main()

hive is the string of letters each word is compared to, to determine its validity (word contains only 
letters from hive)

reqLet is the letter that must be found in the word to be considered valid (in addition to all other letters
also being in the hive)

no return value, populates solveList and updates it for use in main()
*/
void bruteForceSolve(WordList* dictionaryList, WordList* solvedList, char* hive, char reqLet) {
    //iterates through every word in the passed list of words and appends them to the passed (originally empty) list of words
    //if validity criteria is met (only letters from hive and includes required letter)
    for (int i = 0; i < dictionaryList->numWords; i++) {
        if (isValidWord(dictionaryList->words[i], hive, reqLet)) {
            appendWord(solvedList, dictionaryList->words[i]);
        }
    }
}

/*
Determines if a string is the prefix (contains the same initial characters) of another
A prefix can be as small as one character (the first character of the word being compared to)
or as large as the word itself. Characters in prefixes completely match beginning with the 
start of the "full" word being looked at until the end of the "shorter" word being checked as a prefix

partWord is the word being checked as a prefix of another word, it is checked on whether 
it is the "start" of another word

fullWord is the word the prefix is compared to, the word used to check if partWord is a prefix

returns a bool, true if partWord fully matches with the start of fullWord, false if there is any discrepancy
*/
bool isPrefix(char* partWord, char* fullWord) {
    //compares every character in partWord (potential prefix) to the character at the matching index of the full word
    for (int i = 0; i < strlen(partWord); i++) {
        if (partWord[i] != fullWord[i]) {
            return false; //difference found between partWord and fullWord, not a prefix
        }
    }

    return true; 
}

/*
completes a binray search that not only checks whether the searched for word is in the list (matches a word in the list 
or not) but also returns information about whether the word being searched for is a prefix/root of a string in the list.

thisWordList is the list of words being looked at and having its words compared to aWord to see whether aWord fully matches, 
partially matches or does not match a word within it

aWord is the word being searched for in the binary search. aWord can be determined to be a fully match, prefix (partial match),
or not found within the list

loInd is the integer representing the smallest, lower bound index of the search. Updates based on the position of aWord relative
to the know direction of where aWord could be within thisWordList

hiInd is the integer representing the largest, upper bound index of the search. Updates based on the position of aWord relative
to the know direction of where aWord could be within thisWordList

returns an int, either the index at which there was an exact match of aWord in thisWordList, -1 if aWord is not an exact match of
a string in thisWordList but is a prefix of another word in the list, or -99 if there are no matches to aWord and it is not a prefix to another word
*/
int findWord(WordList* thisWordList, char* aWord, int loInd, int hiInd) {
    if (hiInd < loInd) { // Base case 2: aWord not found in words[]

        if (loInd < thisWordList->numWords && isPrefix(aWord, thisWordList->words[loInd])) {
            return -1; //words match this root (partial match)
        }
        else {
            return -99; //no more words matching this root (no match)
        }
    }

    int mdInd = (hiInd + loInd) / 2; //the index at which it will be determined if further recursion is needed. the word at index mdInd is observed

    if (strcmp(aWord, thisWordList->words[mdInd]) == 0) { // Base case 1: found tryWord at midInd
        return mdInd;
    }

    if (strcmp(aWord, thisWordList->words[mdInd]) > 0) { // Recursive case: search upper half
        return findWord(thisWordList, aWord, mdInd + 1, hiInd);
    }

    // Recursive case: search lower half
    return findWord(thisWordList, aWord, loInd, mdInd - 1);
}

/*
Constructs potential words using the letters from the hive to be found in the dictionaryList as solutions. Appends letters
from the hive to the end of the word being searched, updates letters at an index (the last), and removes letters once the path
has been exhausted. 

Covers different possibilities of words built from the hive letters and recursively searches for them using
a binary search that identifies not only perfect matches (between the tested word and a word in the dictionary) but also root words.

Appends results to a list containing all valid letter combinations (solution words) to be used in main()

dictionaryList is the list of words being looked into to find matches to tryWord (the tested word) or prefixes to it. The list
containing all words, both potential solutions and not

solvedList is the, originally empty, list that will contain all words that are solutions to the game/valid in the context of the 
dictionary and hive (in the dictionary, contains the required letter, and only letters from the hive). Passed as a pointer from main() so
it is updated in the function to be used in main()

tryWord is the string being checked for. tryWord is constructed using only hive letters and is compared to words inside dictionaryList in order
to find its exact match or find itself as a prefix of other words in the dictionary

hive is the string containing the game's required letters and what is used to construct tryWord

reqLet is the character that must be in a string (word) for it to be considered valid within that game

no return value, updates solvedList to contain all solutions (valid words) to the game which is then used in main() to display the solutions and other statistics
*/
void findAllMatches(WordList* dictionaryList, WordList* solvedList, char* tryWord, char* hive, char reqLet) {
    int hiveSize = strlen(hive); //not in loop since it remains constant throughout all iterations
   
    //loops as long as tryWord is not an empty string (all valid character combinations have been exhausted)
    while (tryWord[0] != '\0') {
        int curLen = strlen(tryWord);
        //search for try word inside dictionaryList to see if it is an exact match, prefix, or not found word
        int index = findWord(dictionaryList, tryWord, 0, dictionaryList->numWords - 1); 

        if (index >= 0) { //tryWord returned as an exact match
            if (isValidWord(dictionaryList->words[index], hive, reqLet)) { 
                appendWord(solvedList, dictionaryList->words[index]); //if the word is valid, meets (hive conditions) then it is added as a solution (since it is a match as well)   
            }
            //appends the letters of hive to the end of tryWord to search for more possible solutions in case tryWord in this instance was a match and a prefix
            //in adding characters to the end, it allows for other words to be discovered for which tryWord was a prefix to (portion of)  
            for (int i = 0; i < hiveSize; i++) {
                    tryWord[curLen] = hive[i]; //adds letter at current null character index
                    tryWord[curLen + 1] = '\0'; //null-terminates string an index after the last letter
                    findAllMatches(dictionaryList, solvedList, tryWord, hive, reqLet); //compares the new word to those in the dictionary and once again evaluates the result in this function
            }
                
                return; 
        }

        if (index == -1) { //tryWord returned as a prefix

            //appends the letters of hive to the end of tryWord to search for the words tryWord is a suffix of (eventually the whole word will be constructed) 
            //in adding characters to the end, it allows for the words which tryWord is a prefix of to be discovered
            for (int i = 0; i < hiveSize; i++) {
                tryWord[curLen] = hive[i]; //adds letter at current null character index
                tryWord[curLen + 1] = '\0'; //null-terminates string an index after the last letter
                findAllMatches(dictionaryList, solvedList, tryWord, hive, reqLet); //compares the new word to those in the dictionary and once again evaluates the result in this function
            }

            return;
        }
        //if tryWord is not found as a direct match or prefix update the string to be reconstructed/passed recursively in above cases
        //no need to pass recursively here, only updated, would lead to conflicts in the words checked if recursively checked here
        else if (index == -99) { 
            if (tryWord[curLen - 1] != hive[hiveSize - 1]) {  //if the last letter of tryWord is not the last letter of the hive
                tryWord[curLen - 1] = hive[findLetter(hive, tryWord[curLen-1]) + 1]; //set tryWord's last letter equal to the letter in hive one index greater than current
            }
            else { //the last letter if tryWord is the last letter if the hive
                //remove the last letter of tryWord until its last letter is not equal to the last letter of hive
                //also serves as the outer level loop, terminating condition. Loops fails once the lengtt is 0 meaning the only char in tryWord is '\0'; means outer loop will also terminate
                while (curLen > 0 && tryWord[curLen - 1] == hive[hiveSize - 1]) {
                    tryWord[curLen - 1] = '\0';
                    curLen -= 1; //manually update curLen as the string is shortened
                }
                
                //ensures no unpredictable behavior once curLen is 0 with the line below accessing curLen - 1 (index -1)
                if (curLen == 0) {
                    break;
                }

                tryWord[curLen - 1] = hive[findLetter(hive, tryWord[curLen - 1]) + 1]; //update tryWord's last letter equal to the letter in hive one index greater than current
                
            }
        }
    }
}

/*
calculates the total points the player has earned in a given game. 

words are worth 1 point per character unless the word is of exactly the minimum size then it is only worth one point. 
Pangrams (words containing at least one of every letter in the hive) are worth their length in points plus the length of the hive size in points

thisWordList is the list containing the user's correct solutions to the hive for which its elements will have their points calculated

hive is the string containing the letters valid words can be formed from. Used to check whether the words in the passed
list are pangrams (all letters from hive are in the checked word)

returns int, the user's total score based on the sum of the points each solution word earned them
*/
int calculatePoints(WordList* thisWordList, char* hive) {
    int hiveSize = strlen(hive);
    int currWordPoints = 0;
    int totScore = 0;

    for (int wordIndex = 0; wordIndex < thisWordList->numWords; wordIndex++) { //iterates through every word in the list
        currWordPoints = 0;
        currWordPoints = strlen(thisWordList->words[wordIndex]); //all words, other than words of exactly minimum required length earn at least one point per letter

        //checks whether the word is a perfect pangram, all words from hive used exactly once
        if (isPangram(thisWordList->words[wordIndex], hive) && (hiveSize == strlen(thisWordList->words[wordIndex]))) {
            currWordPoints += hiveSize; //pangrams earn hiveSize extra points on top of their length points (for perfect pangrams these values are equal) 
            totScore += currWordPoints; //word score added to total
        }
        //checks whether the word is a pangram, all words from hive used at least once
        else if (isPangram(thisWordList->words[wordIndex], hive) && (hiveSize <= strlen(thisWordList->words[wordIndex]))) {
            currWordPoints += hiveSize; //pangrams earn hiveSize extra points on top of their length points
            totScore += currWordPoints; //word score added to total
        }
        //checks if the word is exactly the minimum required length
        else if (strlen(thisWordList->words[wordIndex]) == MIN_WORD_LENGTH){
            currWordPoints = 1; //only one total point is earned for the word if it is of exactly the minimum length
            totScore += currWordPoints; //word score added to total
        }
        else {
            totScore += currWordPoints; //word score added to total, all other words earn one point per letter
        }
    }

    return totScore; //returns the sum of all points earned from individual words
}

/*
Configures the program with different settings based on command line arguments passed by the user

Possible options include random mode and number of letters, dictonary file, play mode (off means the solver is run without user gameplay),
brute force mode (the unoptimized approach for finding the hive's solution set) which are all respectively passed as pointers 
(except for dictFile which is a character array) in order to update the values in main.  

argc is the number of arguments passed including the compiled code file, argv is these arguments stored as strings into an array
pSeedSelection is the user chosen seed given they choose "random mode", user wont be able to select the letters but the same seed will produce the same list

returns a bool

returns true if all arguments passed are valid, if -r, then a valid pNumLets was given (2 <= x <= 12)
if -d then a valid text file was passed, if -s, then a valid integer is passed, (-p and -o don't require extra arguments)

otherwise, returns false if above conditions are met, or if an invalid argument is passed (i.e. -b). 
If no argument is passed, a default value is used denoted by the parameter assignments at the top of the function
*/
bool setSettings(int argc, char* argv[], bool* pRandMode, int* pNumLets, char dictFile[100], bool* pPlayMode, bool* pBruteForceMode, bool* pSeedSelection) {
    //Default state of all settings
    *pRandMode = false; //user manually selects letters when false
    *pNumLets = 0;
    strcpy(dictFile, "dictionary.txt");
    *pPlayMode = false;
    *pBruteForceMode = true; //optimized search method used when false (-o)
    *pSeedSelection = false;
    srand((int)time(0)); //seed is based on the time, meaning the program would not give the same set of letters in different runs if run with random mode

    for (int i = 1; i < argc; ++i) {
        //checks if random mode is selected
        if (strcmp(argv[i], "-r") == 0) {
            ++i;
            if (argc == i) { //terminates program if no value was provided after -r (what goes into pNumLets)
                return false;
            }
            *pRandMode = true;
            *pNumLets = atoi(argv[i]); //converts argument inputted for pNumLets into an int 
            if (*pNumLets < MIN_HIVE_SIZE || *pNumLets > MAX_HIVE_SIZE) { //terminates program if an invalid number of letters is given 
                return false;
            }
        }
        //checks if the user wanted a specific dictionary text file
        else if (strcmp(argv[i], "-d") == 0) {
            ++i;
            if (argc == i) { //terminates program if no value was provided after -d (what goes into dictFile)
                return false;
            }
            strcpy(dictFile, argv[i]); //copies text file argument into the dictFile variable

            //file is opened, program terminates if selected file is not found 
            FILE* filePtr = fopen(dictFile, "r"); 
            if (filePtr == NULL) {
                return false;
            }
            fclose(filePtr);
        }
        //checks if user wanted to use a specific seed
        else if (strcmp(argv[i], "-s") == 0) {
            ++i;
            if (argc == i) { //terminates program if no value was provided after -s
                return false;
            }
            *pSeedSelection = true;
            int seed = atoi(argv[i]); //sets seed to user inputed integer
            srand(seed);
        }
        //checks if user selected play mode to be on
        else if (strcmp(argv[i], "-p") == 0) { 
            *pPlayMode = true;
        }
        //checks if user selected the optimized search mode to be on
        else if (strcmp(argv[i], "-o") == 0) {
            *pBruteForceMode = false;
        }
        //program terminates if an invalid argument is passed
        else {
            return false;
        }
    }
    return true; //program is configured with user selected and/or default settings and is ready to be used 
}

/*
Displays status of setting as either on (true) or off (false)
mode is the setting who's status is being displayed
no return statement, prints mode status
*/
void printONorOFF(bool mode) {
    if (mode) {
        printf("ON\n");
    }
    else {
        printf("OFF\n");
    }
}

/*
Displays status of setting as either YES (true) or NO (false)
mode is the setting who's status is being displayed
no return statement, prints mode status
*/
void printYESorNO(bool mode) {
    if (mode) {
        printf("YES\n");
    }
    else {
        printf("NO\n");
    }
}

/*
checks if the passed character array (string) is all in lower case letters
str is the string check for all its characters being lower case
returns a bool, true if all characters are lower case, false if at least one character is not lower case
*/
bool isStringLower(char* str) {
    for (int i = 0; i < strlen(str); i++) { //iterates through every character
        if (!islower(str[i])) {
            return false; //a non-lowercase letter is found
        }
    }
    return true; //string is completely lowercase
}

/*
checks that a string does not have any duplicate characters within itself, checks all characters are unique
str is the character array being checked for having all unique characters or not
returns a bool, true if the string has no duplicate characters, false is a duplicate character is found
*/
bool noDuplicateChars(char* str) {
    int len = strlen(str);

    //the inner loop compares every character in the passed string to the character at i+1 (a character does not check itself)
    //checks that every character after itself is not a duplicate, since the process starts from the beginning, there is no need to backtrack
    for (int i = 0; i < len; i++) {
        for (int j = i+1; j < len; j++) { //compare every char to the chars after itself
            if (str[i] == str[j]) {
                return false; //two characters match, a duplicate is found
            }
        }
    }
    return true; //all chars in the string are unique
}

/*
Performs a binary search to find a specficied word in a list. Requires an organized list
to perform the search, splits the search range in half after every iteration and in the known direction of the word
(if it were to theoretically be in the list). 

thisWordList is the list of words where aWord is being searched for in

aWord is the word being searched for within thisWordList

lowVal is the integer representing the smallest, lower bound index of the search. Updates based on the position of aWord relative
to the know direction of where aWord could be within thisWordList

highVal is the integer representing the largest, upper bound index of the search. Updates based on the position of aWord relative
to the know direction of where aWord could be within thisWordList

returns an int, either the index at which aWord was found inside thisWordList or -1 if aWord was not found
*/
int wordInListBinary(WordList* thisWordList, char* aWord, int lowVal, int highVal) {
    
    if (highVal < lowVal) { //Base Case 1: aWord was not found inside thisWordList
        return -1;
    }

    int midVal = (highVal + lowVal) / 2;
    int wordIndex = -1;

    if (strcmp(aWord, thisWordList->words[midVal]) == 0) { //Base Case 2: aWord was found inside thisWordList
        wordIndex = midVal; //midVal is the index at which aWord was found in thisWordList
    }
    else if (strcmp(aWord, thisWordList->words[midVal]) < 0) { //Recursive case 1: aWord is to be found on the lower half of the current range
        wordIndex = wordInListBinary(thisWordList, aWord, lowVal, midVal - 1); //limit range from middle to lower half

    }
    else if(strcmp(aWord, thisWordList->words[midVal]) > 0) { //Recursive case 2: aWord is to be found on the upper half of the current range
        wordIndex = wordInListBinary(thisWordList, aWord, midVal + 1, highVal); //limit range from middle to upper half
    }

    return wordIndex;
}

/*
Performs a linear search to find a specficied word in a list. Checks every element in the list until
a match is found (or not found)

thisWordList is the list which is checked to see if it contains aWord

aWord is the word being searched for within thisWordList to see if the list contains it

returns an int, either the index at which aWord was found inside thisWordList or -1 if aWord was not found
*/
int wordInListLinear(WordList* thisWordList, char* aWord) {
    //iterates through every element in the list
    for (int i = 0; i < thisWordList->numWords; i++) {
        if (strcmp(aWord, thisWordList->words[i]) == 0) { //aWord was found inside thisWordList
            return i;
        }
    }

    return -1; //aWord was not found in thisWordList
}

/*
Displays all words from a given list, particularly used for displaying all the solutions to that game's hive

Provides additional information such as the points earned from the given word and whether the word is a pangram 
or even perfect pangram (pangram (*) contains all letters from hive at least once and perfect pangrams (***) containing every
letter from hive exactly once)

thisWordList is a wordList object, the list being displayed to the user

hive is the string containing the letters valid words can be formed from. Used to check whether the words in the passed
list are pangrams (all letters from hive are in the checked word)

no return value, displays the list alongside additional information such as points earned for said word and the word's status as a pangram
*/
void displaySolverList(WordList* thisWordList, char* hive) {
    int hiveSize = strlen(hive);
    int currWordPoints = 0;

    for (int wordIndex = 0; wordIndex < thisWordList->numWords; wordIndex++) { //iterates through every word in the list
        currWordPoints = 0;
        currWordPoints = strlen(thisWordList->words[wordIndex]); //all words, other than words of exactly minimum required length earn at least one point per letter

        //checks whether the word is a perfect pangram, all words from hive used exactly once
        if (isPangram(thisWordList->words[wordIndex], hive) && (hiveSize == strlen(thisWordList->words[wordIndex]))) {
            currWordPoints += hiveSize;//pangrams earn hiveSize extra points on top of their length points (for perfect pangrams these values are equal)
            printf("*** (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]);
        }
        //checks whether the word is a pangram, all words from hive used at least once
        else if (isPangram(thisWordList->words[wordIndex], hive) && (hiveSize <= strlen(thisWordList->words[wordIndex]))) {
            currWordPoints += hiveSize; //pangrams earn hiveSize extra points on top of their length points
            printf(" *  (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]);
        }
        //checks if the word is exactly the minimum required length
        else if (strlen(thisWordList->words[wordIndex]) == MIN_WORD_LENGTH){
            currWordPoints = 1; //only one total point is earned for the word if it is of exactly the minimum length
            printf("    (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]);
        }
        //all other words
        else {
            printf("    (%d) %s\n", currWordPoints, thisWordList->words[wordIndex]); //all other words earn one point per letter
        }
    }
}

/*
Calculates the number of total pangrams in a given list. A pangram being defined as any word that uses every
letter from the hive at least once.

thisWordList is the list of words for which the number of pangrams it contains is being counted

hive is the string containing the characters needed to make a valid word. Used to see if the word contains all letters
at least once.

returns an int, the number of pangrams found in the list
*/
int calcNumPangrams(WordList* thisWordList, char* hive) {
    int hiveSize = strlen(hive);
    int numPangrams = 0;

    for (int i = 0; i < thisWordList->numWords; i++) { //every word in the list is checked
        if (isPangram(thisWordList->words[i], hive) && (hiveSize <= strlen(thisWordList->words[i]))) {
            numPangrams++; //adds a pangram to the count if the word contains every letter from the hive at least once
        }
    }

    return numPangrams;
}

/*
Calculates the number of total perfect pangrams in a given list. A perfect pangram being defined as any word that uses every
letter from the hive exactly once.

thisWordList is the list of words for which the number of perfect pangrams it contains is being counted

hive is the string containing the characters needed to make a valid word. Used to see if the word contains all letters
exactly once.

returns an int, the number of perfect pangrams found in the list
*/
int calcNumPerfectPangrams(WordList* thisWordList, char* hive) {
    int hiveSize = strlen(hive);
    int numPerfectPangrams = 0;

    for (int i = 0; i < thisWordList->numWords; i++) { //every word in the list is checked
        if (isPangram(thisWordList->words[i], hive) && (hiveSize == strlen(thisWordList->words[i]))) {
            numPerfectPangrams++; //adds a pangram to the count if the word contains every letter from the hive exactly once
        }
    }

    return numPerfectPangrams;
}

/*
Checks if the passed list is a bingo. A bingo is defined as a list that contains at least one word beginning
with each letter of the given hive. If a hive has 4 letters, there are at least four different words each 
starting with a different letter.

thisWordList is the list of words being checked for having a bingo

hive is the string containing all letters that need to be found at the beginning of different words of the list
for the list to be considered a bingo

returns a bool, true if the list is a bingo (as defined above), false if there is at least one character in the hive which is
not the beginning of a word in the list
*/
bool checkBingo(WordList* thisWordList, char* hive) {
    char hiveCopy[strlen(hive) + 1];
    strcpy(hiveCopy, hive); //creates a copy of the hive to be modified
    int hiveLetterIndex;
    int bingoCount = 0; //will count the number of unique starts to words in the list

    for (int i = 0; i < thisWordList->numWords; i++) { //checks every list element
        hiveLetterIndex = findLetter(hiveCopy, thisWordList->words[i][0]); //first letter of the word is searched for in the hive copy
        //if the letter is found inside the hive, then bingoCount is increased and the letter is removed from the hive copy to not be searched for again
        if (hiveLetterIndex != -1) {
            bingoCount++;
            hiveCopy[hiveLetterIndex] = ' ';
        }
    }

    //if the number of different letters found at the beginning of words matches the length (num letters) of hive
    if (bingoCount == strlen(hive)) { 
        return true; //the list is a bingo
    }
    
    return false; //there is at least one character in the hive which is not the beginning of a word in the list
}

int main(int argc, char* argv[]) {

    printf("\n----- Welcome to the CS 211 Spelling Bee Game & Solver! -----\n\n");

    //Initializes all program settings to their defauly configurations
    bool randMode = false;
    int hiveSize = 0;
    char dict[100] = "dictionary.txt";
    bool playMode = false;
    bool bruteForce = true;
    bool seedSelection = false;
    char hive[MAX_HIVE_SIZE + 1] = {}; //hive is given a size of the max size + 1 to accomodate for '\0'
    hive[0] = '\0'; //hive is initialized with no letters
    int reqLetInd = -1;
    char reqLet = '\0';

    //configures program with settings provided as command line arguments, program terminates if an invalid command-line argument is passed
    if (!setSettings(argc, argv, &randMode, &hiveSize, dict, &playMode, &bruteForce, &seedSelection)) {
        printf("Invalid command-line argument(s).\nTerminating program...\n");
        return 1;
    }
    else { //Displays the statuses of the settings the program is running with
        printf("Program Settings:\n");
        printf("  random mode = ");
        printONorOFF(randMode);
        printf("  play mode = ");
        printONorOFF(playMode);
        printf("  brute force solution = ");
        printONorOFF(bruteForce);
        printf("  dictionary file = %s\n", dict); //displays the file being used as the dictionary
        printf("  hive set = "); //Displays whether the hive has been set at this point (YES if random mode on)
        printYESorNO(randMode);
        printf("\n\n");
    }

    // build word array (only words with desired minimum length or longer) from dictionary file
    printf("Building array of words from dictionary... \n");
    WordList* dictionaryList = createWordList(); //initializs a word list with no words in it but a capacity of 4
    int maxWordLength = buildDictionary(dict, dictionaryList, MIN_WORD_LENGTH); //appends all words from the opened text file into the list that meet the dictionary specifications
    if (maxWordLength == -1) { //-1 is returned when the dictionary/word array construction fails
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of valid words.\n");
        printf("Terminating program...\n");
        return -1; //program is terminated if building dictionary list fails
    }
    printf("   Word array built!\n\n");

    printf("Analyzing dictionary...\n");

    //terminate program if the dictionaryList was not properly initialized since default value of numWords is 0 at initialization
    if (dictionaryList->numWords < 0) { 
        printf("  Dictionary %s not found...\n", dict);
        printf("Terminating program...\n");
        return -1;
    }

    // end program if file has zero words of minimum desired length or longer
    if (dictionaryList->numWords == 0) {
        printf("  Dictionary %s contains insufficient words of length %d or more...\n", dict, MIN_WORD_LENGTH);
        printf("Terminating program...\n");
        return -1;
    }
    else { //displays the number of words in the dictionary of a certain length or greater
        printf("  Dictionary %s contains \n  %d words of length %d or more;\n", dict, dictionaryList->numWords, MIN_WORD_LENGTH);
    }

    //sets the game up accordingly if random mode was selected
    if (randMode) {
        printf("==== SET HIVE: RANDOM MODE ====\n");
        //find number of words in words array that use hiveSize unique letters
        WordList* fitWords = findAllFitWords(dictionaryList, hiveSize);
        int numFitWords = fitWords->numWords;
        //pick one at random, this will be the word the hive is constructed from
        int pickOne = rand() % numFitWords;
        char* chosenFitWord = fitWords->words[pickOne];

        //and alaphabetize the unique letters to make the letter hive
        buildHive(chosenFitWord, hive);
        freeWordList(fitWords); //frees heap-allocated memory containing the words with hiveSize unique letters as it is no longer needed

        reqLetInd = rand() % hiveSize; //randomly selects one of the indexes of the selected letters to be the required letter, stores index
        reqLet = hive[reqLetInd]; //stores the required character based on the randomly selected index

    }
    else {
        //Users builds the hive through characters they input rather than it being randomly generated
        printf("==== SET HIVE: USER MODE ====\n");

        printf("  Enter a single string of lower-case,\n  unique letters for the letter hive... ");
        scanf(" %s", hive); //user is prompted to construct hive

        //Ask until the hive inputted is within the specified size range, all in lowercase and contains no duplicate characters
        while ((strlen(hive) > MAX_HIVE_SIZE || strlen(hive) < MIN_HIVE_SIZE) || (!isStringLower(hive)) ||  (!noDuplicateChars(hive)) ) {
            if (strlen(hive) > MAX_HIVE_SIZE || strlen(hive) < MIN_HIVE_SIZE) {
                printf("  HIVE ERROR: \"%s\" has invalid length;\n  valid hive size is between %d and %d, inclusive\n\n",hive, MIN_HIVE_SIZE, MAX_HIVE_SIZE);
            }
            else if (!isStringLower(hive)) {
                printf(" HIVE ERROR: \"%s\" contains invalid letters;\n  valid characters are lower-case alpha only\n\n",hive);
            }
            else if (!noDuplicateChars(hive)) {
                printf("  HIVE ERROR: \"%s\" contains duplicate letters\n\n",hive);
            }

            printf("  Enter a single string of lower-case,\n  unique letters for the letter hive... ");
            scanf(" %s",hive);
        }

        hiveSize = strlen(hive); //determines hive size from the number of letters inputted by the user
        
        buildHive(hive, hive);

        //Default values for the required letter, not set yet
        reqLetInd = -1;
        reqLet = '\0';

        printf("  Enter the letter from \"%s\"\n  that is required for all words: ", hive);
        scanf(" %c", &reqLet);
        reqLet = tolower(reqLet); //letter may be inputted as upercase by user, this does not matter

        //repeat until user enters a letter that is in the hive
        while (findLetter(hive, reqLet) == -1) {
            printf("  HIVE ERROR: \"%s\" does not contain the character \'%c\'\n\n",hive,reqLet);

            printf("  Enter the letter from \"%s\"\n  that is required for all words: ", hive);
            scanf(" %c", &reqLet);
            reqLet = tolower(reqLet);
        }

        reqLetInd = findLetter(hive, reqLet); //set the index of the letter by searching for the inputted character in the hive
    }

    printHive(hive, reqLetInd); //displays completed hive ready to be played with containing all information the user needs

    //Entered if user selected play mode, user may play the spelling bee game using the constructed hive
    if (playMode) {
        printf("==== PLAY MODE ====\n");

        char* userWord = (char*)malloc((maxWordLength + 1) * sizeof(char)); //allocates enough space for the longest possible word a user could give as a valid answer and '\0' 
        strcpy(userWord, "default"); //initializes user word with a default value
        int userPointTarget = 0; //user selected point goal for their game
        int totalScore = 0; //total user score, needs to be greater than or equal to userPointTarget to complete game
        int scoreWithMultiplier = 0; //user score calculated from point target and a time multiplier (target * (120 seconds - final time) + target)
        int localHighscore; //highest score saved within local files, calculated from point target and a time multiplier
        time_t gameStartTime; //keeps track of start time to find duration of user's game
        double userFinalTime; //the user's final time, calculated by subtracting their start time from their final time (final - initial)

        WordList* userWordList = createWordList(); //userWordList keeps track of correct user answers (user answers that meet all criteria)

        printf("............................................\n");
        printHive(hive, reqLetInd);

        printf("  Select a valid target point value (10 to 150): ");
        scanf("%d", &userPointTarget);
        
        //loops until user enters a number in the specified range
        while (userPointTarget < 10 || userPointTarget > 150) {
            printf("  Select a valid target point value (10 to 150): ");
            scanf("%d", &userPointTarget);
        }
       
        gameStartTime = time(NULL); //creates a time snapshot of when the user is first able to input solutions (start time)

        printf("  Enter a word (enter DONE to quit): ");
        scanf("%s", userWord); //user may begin providing solutions to the hive
        printf("\n");

        //iterates until user types "DONE" must be in in all capital letters and as long as the user does not achieve or surpass their chosen point target
        while (strcmp(userWord, "DONE") && totalScore < userPointTarget) { 

            //if the word is valid (contains only letters from the hive and uses the required letter) and is found in the game's dictionary
            //and the word is not repeated/has not been found by the user yet then add the word to the user's solutions list
            if (isValidWord(userWord, hive, reqLet) && 
                wordInListBinary(dictionaryList, userWord, 0, dictionaryList->numWords) != -1 &&
                (userWordList->numWords == 0 || wordInListLinear(userWordList, userWord) == -1)) {

                appendWord(userWordList, userWord);
            }
            //otherwise present the corresponding error message
            else {
                if (!isValidWord(userWord, hive, reqLet)) {
                    printf("  WORD CHOICE ERROR: \"%s\" is not composed of only hive letters and/or is missing the required letter \'%c\'\n\n", userWord, reqLet);
                }
                else if (wordInListBinary(dictionaryList, userWord, 0, dictionaryList->numWords) == -1) {
                    printf("  WORD CHOICE ERROR: \"%s\" is not a valid word in the dictionary\n\n", userWord);
                }
                else if (wordInListLinear(userWordList, userWord) != -1) {
                    printf("  WORD CHOICE ERROR: \"%s\" was already found!\n\n", userWord);
                }
            }

            //Displays the user's found word list
            printf("\n");
            printList(userWordList, hive); //displays the user's correct answers along with point values and other qualities each iteration
            printf("\n");
            totalScore = calculatePoints(userWordList, hive); //calculates user's total points to keeping track of how far away the user is from completing the game

            //save user's final time as the point target has been met, loop will exit at the condition check
            if (totalScore >= userPointTarget) { 
                userFinalTime = difftime(time(NULL), gameStartTime); //calculates the difference between final and initial time resulting in total elapsed time
            }
            else { //Displays hive, and gets the next input
                printf("............................................\n");
                printHive(hive, reqLetInd);
                printf("  Enter a word, only %d more points to go! (enter DONE to quit): ", userPointTarget - totalScore);
                scanf("%s", userWord);
                printf("\n");
            }
        }

        //Only display time/score if user reached the point target and now quit the game with "DONE" 
        if (totalScore >= userPointTarget) {
            FILE* highscoreFilePtr = fopen("local_highscore.txt", "r");
            
            fscanf(highscoreFilePtr, "%d", &localHighscore); //extract currently saved high score for comparisons

            //if the user took more than two minutes then their score is just the points they earned from reaching the target
            if (userFinalTime > 120) {
                scoreWithMultiplier = totalScore;
            }
            //otherwise, multiply the target the user reached times the amount of time remaining from the given 2 minute timer, add the target score to this result
            else {
                scoreWithMultiplier = userPointTarget * (120 - userFinalTime) + userPointTarget;
            }
            
            //diplay final time, current local highscore, and update file accordingly
            printf("  Congratulations! You finished in %.2lf seconds\n", userFinalTime);

            if (scoreWithMultiplier > localHighscore) {
                printf("  You set a NEW HIGHSCORE!\n");
                printf("  Previous Highscore: %d\n", localHighscore);
                printf("  New Highscore: %d\n", scoreWithMultiplier);

                //open the file in writing mode to update the value if the user surpassed the saved high score
                freopen("local_highscore.txt", "w", highscoreFilePtr);
                fprintf(highscoreFilePtr, "%d", scoreWithMultiplier);
                fclose(highscoreFilePtr); //ensure file is closed regardless of score result
            }
            else if (scoreWithMultiplier == localHighscore) {
                printf("  You Were So Close! You Tied The Highscore!\n");
                printf("  Current Highscore: %d\n", localHighscore);
                printf("  Your Score: %d\n", scoreWithMultiplier);
                fclose(highscoreFilePtr); //ensure file is closed regardless of score result
            }
            else { //did not meet or surpass saved high score
                printf("  Better Luck Next Time!\n");
                printf("  Current Highscore: %d\n", localHighscore);
                printf("  Your Score: %d\n", scoreWithMultiplier);
                fclose(highscoreFilePtr); //ensure file is closed regardless of score result
            }
        }

        //free heap-allocated memory after user is done playing
        freeWordList(userWordList); 
        free(userWord);
    }
    
    printf("==== SPELLING BEE SOLVER ====\n");
    
    //Displays hive and highlights required letter
    printf("  Valid words from hive \"%s\":\n", hive);
    printf("                         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^\n");

    WordList* solvedList = createWordList(); //contains all solutions using the letters from the hive and required letter

    if (bruteForce) { //find all words that work by checking each individual word in the dictionary and verifying it meets the hive's conditions
        bruteForceSolve(dictionaryList, solvedList, hive, reqLet);
    }
    else { //finds all words that work through an optimized search algorithm (using a modified binary search), entered if -o was used as command line argument
        char* tryWord = (char*)malloc(sizeof(char) * (maxWordLength + 1)); //tryWord is allocated with enough space for the longest word possible from the dictionary and '\0' 

        //tryWord is passed to findAllMatches() as the first character of the hive, the search word will be constructed from hive letters only
        tryWord[0] = hive[0];
        tryWord[1] = '\0';
        findAllMatches(dictionaryList, solvedList, tryWord, hive, reqLet);
        free(tryWord); //frees tryWord after optimized algorithm has completed the solution list

    }

    // Helpful variables
    int numValidWords = 0;
    int numPangrams = 0;
    int numPerfectPangrams = 0;
    int totScore = 0;
    bool isBingo = true;

    //Displays the list containing all solutions to the hive (all valid words based on the hive) from the dictionary
    //Notes pangrams (*) and perfect pangrams (***)
    displaySolverList(solvedList, hive);

    numValidWords = solvedList->numWords; //number of valid words is simply the size of the solution list
    numPangrams = calcNumPangrams(solvedList, hive); //pangram is defined as a word that has at least one of every letter in the hive
    numPerfectPangrams = calcNumPerfectPangrams(solvedList, hive); //perfect pangram is defined as a word that has exactly one of every letter in the hive
    totScore = calculatePoints(solvedList, hive); 
    isBingo = checkBingo(solvedList, hive); //A bingo is defined as a list that contains at least one word beginning with each letter of the given hive

    // Additional results are printed here as well as hive and highlight of required letter
    printf("\n");
    printf("  Total counts for hive \"%s\":\n", hive);
    printf("                         ");
    for (int i = 0; i < reqLetInd; i++) {
        printf(" ");
    }
    printf("^\n");

    printf("    Number of Valid Words: %d\n", numValidWords);
    printf("    Number of ( * ) Pangrams: %d\n", numPangrams); //pangram is defined as a word that has at least one of every letter in the hive
    printf("    Number of (***) Perfect Pangrams: %d\n", numPerfectPangrams); //perfect pangram is defined as a word that has exactly one of every letter in the hive
    printf("    Bingo: "); //A bingo is defined as a list that contains at least one word beginning with each letter of the given hive
    printYESorNO(isBingo);
    printf("    Total Score Possible: %d\n", totScore);

    //All heap-allocated memory not yet freed, is freed before program termination
    freeWordList(dictionaryList);
    freeWordList(solvedList);                                            
    printf("\n\n");
    return 0;
}