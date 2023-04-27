#include <stdio.h>
#include <stdlib.h>
#include <ncurses/ncurses.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define BOXHEIGHT 25
#define BOXWIDTH 80
#define STARTY 0
#define STARTX 0

//mvhline()
//mvvline()
//mvaddch(y, x, ACS_UL/LL/UR/LRCORNER);

struct timespec loop = {0, 8333333};
struct timespec pause = {5, 0};

typedef struct word_struct {
    int y;
    int x;
    char* word;
} word;

void printbox();
int number_of_words();
void scan_file(int n, char** wordlistf);
void spawnWord(int n, word* spawnedWord, char** wordlistf);

int main(void){
    srand(time(NULL));
    initscr();
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
    while (gamestate){
        for (int u = 0; u < nPrinted; u += 1){
            for (int o = 0; o < strlen(spawnedWords[u].word); o += 1){
                mvaddch(spawnedWords[u].y - 1, spawnedWords[u].x + o, ' ');
            }
            mvaddstr(spawnedWords[u].y, spawnedWords[u].x, spawnedWords[u].word);
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
        move(BOXHEIGHT + 1,5);
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
    spawnedWord->x = ((rand() % BOXWIDTH - 3) + 3) - strlen(wordlistf[chosenWord]);
    spawnedWord->y = 3;
}