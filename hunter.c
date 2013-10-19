#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define MAX_MESSAGE_SIZE 128

// static int possibleDraculaLocations(HunterView hView, possible[NUM_LOCATIONS]);

void generateMessage(HunterView hView, char *message);
char **getDraculaTrails(char **draculaPaths, int *numPaths);
void getBestMove(char *bestMove, char **draculaPaths, int numPaths);


void decideMove(HunterView hView) {
    // backup "default" move for the start
    // does this move have to be a legal move?
    char bestMove[3] = "ZU";
    registerBestPlay(bestMove, message);

    // BM the crap out of other hunters
    char message[MAX_MESSAGE_SIZE];
    generateMessage(hView, message);
    
    // Begin analysing the information we have, incrementally analysing deeper
    // Get initial trails of length 0 (1 city)
    int numPaths;
    char **draculaTrails = getDraculaTrails(draculaPaths, &numPaths, 0);
    int depth = 1; // how deep to take the analysis
    while (TRUE) {
        // Use previous dracula trails to incrementally generate more
        draculaTrails = getDraculaTrails(draculaTrails, &numPaths, depth);
        
        // Use all possible dracula trails to evaluate a best move
        getBestMove(bestMove, draculaTrails, numPaths);
        
        free(draculaTrails);        
        // Finally, register best move and message
        registerBestPlay(bestMove, message);
    }
}

void generateMessage (HunterView hView, char *message) {
    if (getRound(hView) == 0) {
        strcpy(message, "mid or feed");
    } else if (getScore(hView) < 100) {
        strcpy(message, "noob team");
    } else if (getScore(hView) < 200) {
        strcpy(message, "semi-retard team");
    } else {
        strcpy(message, "you guys aren't terrible");
    }
}

char **getDraculaTrails(char trail[TRAIL_SIZE], int *numPaths, int lengthTrail) {
    // Accepts trails of length n as input, and generates trails of length n + 1 as output
    
    return NULL;
}

void getBestMove(char *bestMove, char **draculaPaths, int numPaths) {
    // Uses the relevant trail data to predict where dracula could move next, and have the
    // hunter move accordingly
    return;
}

/*
// calculates all of Dracula's possible locations
// returns 0 if no useful data available
// returns 1 if some result is achieved
static int possibleDraculaLocations(HunterView hView, possible[NUM_LOCATIONS]) {
    int i;
    // set all locations to false first
    for (i = 0; i < NUM_LOCATIONS; i++) {
        possible[i] = FALSE;
    }
    
    // get Dracula's trail
    LocationID trail[TRAIL_SIZE];
    getHistory(hView, PLAYER_DRACULA, trail);
    
    // see if there is useful info in Dracula's trail
    // and if so, find where it starts
    // NOTE: useful info is classified as either a known for sure location
    //       or an unknown sea (since those are relatively few)
    int usefulData = FALSE;
    for (i = TRAIL_SIZE - 1; i >= 0; i--) {
        if (trail[i] == SEA_UNKNOWN) {
            usefulData = TRUE;
            break;
        } else if (trail[i] == UNKNOWN_LOCATION ||
            (trail[i] >= CITY_UNKNOWN && trail[i] <= DOUBLE_BACK_5)) {
            continue;
        } else {
            usefulData = TRUE;
            break;
        }
    }
    int startUsefulData = i;
    
    for (i = startUsefulData; i >= 0; i--) {
        if (trail[i] <= BLACK_SEA) {
            // Dracula is in a known location
            int j;
            for (j = 0; j < NUM_LOCATIONS; j++) {
                possible[j] = FALSE;
            }
            possible[trail[i]] = TRUE;
        }
        int j;
        for (j = 0; j < NUM_LOCATIONS; j++) {
            // for each of the possible loactions Dracula was in last turn
            if (possible[j]) {
                // NOTE: need to find a good way of unmarking city j as possible
                // TODO: take into account past trail
                if (trail[i] == CITY_UNKNOWN) {
                    // TODO: every city adjacent to j should be marked as possible
                } else if (trail[i] == SEA_UNKNOWN) {
                    // TODO: every sea adjacent to j should be marked as possible
                }
            }
        }
    }
    return usefulData;
}
*/                 
