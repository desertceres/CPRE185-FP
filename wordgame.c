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


#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h> //Arch doesn't have ncurses-dev package folder
//#include <ncurses/ncurses.h>
//#include <curses/ncurses.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define BOXHEIGHT 25
#define BOXWIDTH 120
#define STARTY 0
#define STARTX 0
#define SCOREBOARDHEIGHT 2
#define SCOREBOARDWIDTH 3
#define TIMEHEIGHT 2
#define TIMEWIDTH 4
#define CHANCETIMESEG 20

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
//User Defined Function Prototypes//
////////////////////////////////////
void drawBox();
int number_of_words();
void scan_file(int n, char** wordlistf);
void spawnWord(int n, word* spawnedWord, char** wordlistf);
void clearTypingArea();
void endMessage(int sTime);
void drawScoreboard(int userScore);
void drawTime(int sTime);
void addWords(char addWord[100]);


int main(void){
    char begin;
	printf("Welcome to the word game.\nEnter 'c' to begin.\nOr enter 'a' to add words --> ");
	scanf("%c", &begin);
	
	if (begin == 'c' || begin == 'C') {
	} else if (begin == 'a' || begin == 'A') {
		char YorN = 'y';
		do {
			char temp[100];
			printf("Enter word to add --> ");
			scanf("%s", temp);
			addWords(temp);
			printf("Want to add another? ('y' or 'n') --> ");
			scanf(" %c", &YorN);
		} while (YorN == 'y');
		
	} else { return 0; }

    //Scan file into wordlist
    int numberOfWords = number_of_words();
    char** wordlist = (char**)malloc(numberOfWords * sizeof(char*));
    scan_file(numberOfWords, wordlist);

    srand(time(NULL));      //Window initialization and rand seed
    WINDOW *w = initscr();
    cbreak();
    noecho();
    nodelay(w, TRUE);

    drawBox();

    //Setup loop variables
    int startTime = time(NULL); //Start time
    word spawnedWords[3000]; //Array for words actively on board
    int nPrinted = 0; //Number of words on board
    bool gamestate = true; //Gamestate is true until failure condition
    int cycle = 1;  //Cycle counter, game runs 120 cycles per second
    char userWord[50] = " "; //Array for word that user is typing, set to blank by default
    int userWordAddIndex = 0; //Index for the next index in the array that the user will be adding to when typing
    char input; //Input char to store value from getch()
    cursor mainCursor = {BOXHEIGHT + 1, 3};  //Struct for location of cursor in typing area
    int chance = 10; //For changing chance of words spawning over time
    int score = 0;  //User score, starts at 0
    int cycleChance = 0; //Initialize cycle chance variable


    /*--------------------------------------------Main Game Loop--------------------------------------------*/
    while (gamestate){
        chance = time(NULL) - startTime; //Chance of word spawn ticks up every second
        cycleChance = 120 / (((time(NULL) - startTime) / CHANCETIMESEG) + 2); //Calculates the number of times per second
        mainCursor.y = BOXHEIGHT + 1;                                         //there is a chance for a word to spawn by using cycle count
        mainCursor.x = 3;
        drawScoreboard(score);
        drawTime(startTime);
        for (int a = 0; a < nPrinted; a += 1){ //Checks for fail condition
            if (spawnedWords[a].y >= BOXHEIGHT){
                gamestate = false;
            }
        }
        if (nPrinted > 0 && !(spawnedWords[0].word == NULL)){ //Checks if words are printed before iterating through
            for (int c = 0; c < nPrinted; c += 1){  //Loop checks every currently active word against user's word to look for match
                if (strcmp(userWord, spawnedWords[c].word) == 0){
                    for (int v = 0; v < strlen(spawnedWords[c].word); v += 1){ //If match, word is erased
                        mvaddch(spawnedWords[c].y, spawnedWords[c].x + v, ' ');
                    }
                    for (int m = c; m < nPrinted - 1; m += 1){ //Every word is shifted left in array by 1
                        strcpy(spawnedWords[m].word, spawnedWords[m + 1].word);
                        spawnedWords[m].y = spawnedWords[m + 1].y;
                        spawnedWords[m].x = spawnedWords[m + 1].x;
                    }
                    score += 1; //Score increases by 1
                    nPrinted -= 1; //Number of active words decreases by 1
                    strcpy(userWord, " "); //User's input word is reset
                    userWordAddIndex = 0; //User's input word index is reset
                    clearTypingArea(); //Typing area is cleared
                }
            }
            }
        for (int u = 0; u < nPrinted; u += 1){
            for (int o = 0; o < strlen(spawnedWords[u].word); o += 1){
                mvaddch(spawnedWords[u].y - 1, spawnedWords[u].x + o, ' '); //Erases spot above word
            }
            mvaddstr(spawnedWords[u].y, spawnedWords[u].x, spawnedWords[u].word); //Prints word at new location
        }
        if ((input = getch()) != ERR){  //Checks to see if user is pressing a key
            clearTypingArea();
            if ((input == 127 || input == 8) && strlen(userWord) > 0){ //Handles backspace
                userWord[userWordAddIndex - 1] = '\0';
                userWordAddIndex -= 1;
            } else if (input == 9){ //Handles exiting program
                gamestate = false;
            } else {
            userWord[userWordAddIndex] = input;
            userWord[userWordAddIndex + 1] = '\0';
            userWordAddIndex += 1;
            }
        }
        /*mvaddch(BOXHEIGHT+5, BOXWIDTH + 5, ' ');  //Debug display for cycleChance variable
        mvaddch(BOXHEIGHT+5, BOXWIDTH + 6, ' ');
        mvaddch(BOXHEIGHT+5, BOXWIDTH + 7, ' ');
        mvprintw(BOXHEIGHT + 5, BOXWIDTH + 5, "%d", cycleChance);*/
        if (cycle % cycleChance == 0){ //Cycle count mod cycleChance ie. if cycleChance is 40, then this statement
            if ((rand() % 50) < chance){ //executes 3 times a second
                spawnWord(numberOfWords, &spawnedWords[nPrinted], wordlist);
                nPrinted += 1;
            }
        }
        if (cycle >= 120){ //Every 1 second (120 cycles) all words are shifted down by 1
            for (int t = 0; t < nPrinted; t += 1){
                spawnedWords[t].y += 1;
            }
            cycle = 1; //Cycle is reset every 120 cycles (1-120)
        }
        for (int b = 0; b < strlen(userWord); b += 1){ //Print user's word to typing area
                mvaddstr(BOXHEIGHT + 1, 2, userWord);
        }
        refresh();
        cycle += 1;
        nanosleep(&loop, NULL); //This controls how many times per second the loop executes
    }

    for (int h = 0; h < numberOfWords; h += 1){ //Loop to free wordlist array's words
        free(wordlist[h]);
    }
    free(wordlist);  //Free wordlist array
    endMessage(startTime);
    endwin();
    return 0;
}

///////////////////////////////////////
//User Defined Function Definitions//
/////////////////////////////////////
int number_of_words(){ //Scans number of words in wordlist to set size of wordlist array
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

void scan_file(int n, char** wordlistf){ //Scans words into wordlist
    FILE* fp2 = fopen("wordlist.txt", "r");
    for (int i = 0; i < n; i += 1){
        char* tempString = malloc(20 * sizeof(char));
        fscanf(fp2, " %s", tempString);
        wordlistf[i] = tempString;
    }
    fclose(fp2);
}

void spawnWord(int n, word* spawnedWord, char** wordlistf){ //Chooses random word, and random x location to spawn word
    int chosenWord = rand() % n;
    spawnedWord->word = wordlistf[chosenWord];
    spawnedWord->x = (rand() % ((BOXWIDTH - 3) - strlen(wordlistf[chosenWord]))) + 2; //Shortens spawn area by 1 more than length of 
    spawnedWord->y = 2;                                                             //chosen word, then shifts left by 1 to avoid left wall
}

void clearTypingArea(){ //Clears user's typing area
    for (int g = 2; g < BOXWIDTH - 2; g += 1){
        mvaddch(BOXHEIGHT + 1, g, ' ');
    }
}

void endMessage(int sTime){ //Handles game end
    clear();
    drawBox();
    mvprintw(BOXHEIGHT / 2, (BOXWIDTH / 2) - 11, "You made it %d seconds!", time(NULL) - sTime);
    refresh();
    nanosleep(&pause, NULL); //Pauses so game does not end instantaneously
}

void drawBox(){ //Draws game box
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

void drawScoreboard(int userScore){ //Draws user score box
    int scoreboardy = 3;
    int scoreboardx = BOXWIDTH + 3;
    mvaddstr(2, scoreboardx, "SCORE");
    mvprintw(scoreboardy + 1, scoreboardx + 1, "%02d", userScore);
    mvhline(scoreboardy, scoreboardx, 0, SCOREBOARDWIDTH);
    mvhline(scoreboardy + SCOREBOARDHEIGHT, scoreboardx, 0, SCOREBOARDWIDTH);
    mvvline(scoreboardy, scoreboardx, 0, SCOREBOARDHEIGHT);
    mvvline(scoreboardy, scoreboardx + SCOREBOARDWIDTH, 0, SCOREBOARDHEIGHT);
    mvaddch(scoreboardy, scoreboardx, ACS_ULCORNER);
    mvaddch(scoreboardy, scoreboardx + SCOREBOARDWIDTH, ACS_URCORNER);
    mvaddch(scoreboardy + SCOREBOARDHEIGHT, scoreboardx, ACS_LLCORNER);
    mvaddch(scoreboardy + SCOREBOARDHEIGHT, scoreboardx + SCOREBOARDWIDTH, ACS_LRCORNER);
}

void drawTime(int sTime){ //Draws box for time display and press tab to quit message
    int timey = 7;
    int timex = BOXWIDTH + 3;
    mvaddstr(6, timex, "TIME");
    mvprintw(timey + 1, timex + 1, "%03d", time(NULL) - sTime);
    mvhline(timey, timex, 0, TIMEWIDTH);
    mvhline(timey + TIMEHEIGHT, timex, 0, TIMEWIDTH);
    mvvline(timey, timex, 0, TIMEHEIGHT);
    mvvline(timey, timex + TIMEWIDTH, 0, TIMEHEIGHT);
    mvaddch(timey, timex, ACS_ULCORNER);
    mvaddch(timey, timex + TIMEWIDTH, ACS_URCORNER);
    mvaddch(timey + TIMEHEIGHT, timex, ACS_LLCORNER);
    mvaddch(timey + TIMEHEIGHT, timex + TIMEWIDTH, ACS_LRCORNER);
    mvaddstr(timey + 5, timex, "Press tab to quit.");
}

void addWords(char addWord[100]) {
	FILE* file = fopen("wordlist.txt", "a");
	fprintf(file, "\n%s", addWord);
	fclose(file);
}


//gcc ./wordgame.c -o test -lncurses
// ./test
