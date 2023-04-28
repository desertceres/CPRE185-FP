//SE 185: Final Project Template//
/////////////////////////
/* 
Team member 1 "Grant Nagel" | 20%
Team member 2 "Preston Gaskill" | 20%
Team member 3 "Joseph Hennings" | 20%
Team member 4 "Jack Morrison" | 20%
Team member 5 "John Carber" | 20%
*/
////////////////////

//Needs to be added: Beginning message, adding words to wordlist, quit button, word spawn chance increases as time goes on, track user time
//lose condition,

#include <stdio.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define BOXHEIGHT 25
#define BOXWIDTH 120
#define STARTY 0
#define STARTX 0

//mvhline()
//mvvline()
//mvaddch(y, x, ACS_UL/LL/UR/LRCORNER);

//////////////////////
//Struct Definition//
////////////////////
struct timespec loop = {0, 8333333};
struct timespec pause = {5, 0};

typedef struct word_struct {
    int y;
    int x;
    char* word;
} word;

typedef struct cursor_struct {
    int y;
    int x;
} cursor;

/////////////////////////////////////
//User Defined Functions Prototype//
//List prototypes here and define//
//tehm below the main function////
/////////////////////////////////
void printbox();
int number_of_words();
void scan_file(int n, char** wordlistf);
void spawnWord(int n, word* spawnedWord, char** wordlistf);
void clearTypingArea();


int main(void){
    srand(time(NULL));
    WINDOW *w = initscr();
    cbreak();
    noecho();
    nodelay(w, TRUE);


    printbox();


    int numberOfWords = number_of_words();
    char** wordlist = (char**)malloc(numberOfWords * sizeof(char*));
    scan_file(numberOfWords, wordlist);
    word spawnedWords[BOXHEIGHT];
    int* currentWordIndex = (int*)malloc(sizeof(int));
    *currentWordIndex = 0;
    int nPrinted = 0;
    int temp = 0;
    bool gamestate = true;
    int cycle = 0;
    char userWord[50] = " ";
    int userWordAddIndex = 0;
    char input;
    cursor mainCursor = {BOXHEIGHT + 1, 3};
    int activeWord = 0;


    while (gamestate){
        mainCursor.y = BOXHEIGHT + 1;
        mainCursor.x = 3;
        for (int u = 0; u < nPrinted; u += 1){
            for (int o = 0; o < strlen(spawnedWords[u].word); o += 1){
                mvaddch(spawnedWords[u].y - 1, spawnedWords[u].x + o, ' ');
            }
            mvaddstr(spawnedWords[u].y, spawnedWords[u].x, spawnedWords[u].word);
        }
        if ((input = getch()) != ERR){
            clearTypingArea();
            if ((input == 127 || input == 8) && strlen(userWord) > 0){
                userWord[userWordAddIndex - 1] = '\0';
                userWordAddIndex -= 1;
            } else {
            userWord[userWordAddIndex] = input;
            userWord[userWordAddIndex + 1] = '\0';
            userWordAddIndex += 1;
            if (nPrinted > 0 && !(spawnedWords[0].word == NULL)){
                if (strcmp(userWord, spawnedWords[0].word) == 0){
                    for (int v = 0; v < strlen(spawnedWords[0].word); v += 1){
                        mvaddch(spawnedWords[0].y, spawnedWords[0].x + v, ' ');
                    }
                    for (int m = 0; m < nPrinted - 1; m += 1){
                        strcpy(spawnedWords[m].word, spawnedWords[m + 1].word);
                        spawnedWords[m].y = spawnedWords[m + 1].y;
                        spawnedWords[m].x = spawnedWords[m + 1].x;
                    }
                    nPrinted -= 1;
                    strcpy(userWord, " ");
                    userWordAddIndex = 0;
                }
            }
            }
        }
        if (cycle >= 120){
            if ((rand() % 2) == 1){
                spawnWord(numberOfWords, &spawnedWords[nPrinted], wordlist);
                nPrinted += 1;
            }
            for (int t = 0; t < nPrinted; t += 1){
                spawnedWords[t].y += 1;
            }
            cycle = 0;
        }
        for (int b = 0; b < strlen(userWord); b += 1){
                mvaddstr(BOXHEIGHT + 1, 2, userWord);
        }
        refresh();
        cycle += 1;
        nanosleep(&loop, NULL);
    }




    nanosleep(&pause, NULL);
    for (int h = 0; h < numberOfWords; h += 1){
        free(wordlist[h]);
    }
    free(wordlist);
    endwin();
    return 0;
}

///////////////////////////////////////
//User Defined Functions' Definition//
/////////////////////////////////////
void printbox(){
    mvhline(STARTY, STARTX, 0, BOXWIDTH);
    mvhline(STARTY + BOXHEIGHT, STARTX, 0, BOXWIDTH);
    mvhline(STARTY + BOXHEIGHT + 2, STARTX, 0, BOXWIDTH);
    mvvline(STARTY, STARTX, 0, BOXHEIGHT + 2);
    mvvline(STARTY, STARTX + BOXWIDTH, 0, BOXHEIGHT + 2);
    mvaddch(STARTY, STARTX, ACS_ULCORNER);
    mvaddch(STARTY, STARTX + BOXWIDTH, ACS_URCORNER);
    mvaddch(STARTY + BOXHEIGHT + 2, STARTX, ACS_LLCORNER);
    mvaddch(STARTY + BOXHEIGHT + 2, STARTX + BOXWIDTH, ACS_LRCORNER);
    mvaddch(STARTY + BOXHEIGHT, STARTX, ACS_LTEE);
    mvaddch(STARTY + BOXHEIGHT, STARTX + BOXWIDTH, ACS_RTEE);
    refresh();
}

int number_of_words(){
    FILE* fp = fopen("wordlist.txt", "r");
    char placeholder[50];
    int wordcount = 0;
    while (!feof(fp)){
        fscanf(fp, "%s", placeholder);
        wordcount += 1;
    }
    fclose(fp);
    return wordcount;
}

void scan_file(int n, char** wordlistf){
    FILE* fp2 = fopen("wordlist.txt", "r");
    for (int i = 0; i < n; i += 1){
        char* temp = malloc(20 * sizeof(char));
        fscanf(fp2, " %s", temp);
        wordlistf[i] = temp;
    }
    fclose(fp2);
}

void spawnWord(int n, word* spawnedWord, char** wordlistf){
    int chosenWord = rand() % n;
    spawnedWord->word = wordlistf[chosenWord];
    spawnedWord->x = (rand() % (BOXWIDTH - strlen(wordlistf[chosenWord])) + 3);
    spawnedWord->y = 3;
}

void clearTypingArea(){
    for (int g = 2; g < BOXWIDTH - 2; g += 1){
        mvaddch(BOXHEIGHT + 1, g, ' ');
    }
}

//gcc ./wordgame.c -o test -lncurses
// ./test
