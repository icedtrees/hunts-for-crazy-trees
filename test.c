#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include "locations.h"
#include "dracula.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

void printDraculaTrails(LocationID **trails, int numTrails) {
    int i;
    for (i = 0; i < numTrails; i ++) {
        int j;
        for (j = 0; j < TRAIL_SIZE; j ++) {
            if (trails[i][j] == UNKNOWN_LOCATION) {
                printf("X");
            } else {
                printf("%s", names[trails[i][j]]);
            }
            if (j != TRAIL_SIZE - 1) {
                printf("<--");
            }
        }
        printf("\n");
    }
}

void registerBestPlay( char *play, playerMessage message ) {}

int main(int argc, char *argv[]) {
    playerMessage messages[] = {};
    char *pastPlays = "GJM.... SHA.... HGO.... MTO.... DLO.V.. GSJ.... SNS.... HFL.... MSR.... DC?T... GZA.... SED.... HVE.... MSN.... DC?T... GSZ.... SMN.... HMU.... MAL.... DS?.... GKL.... SLOV... HLI.... MLS.... DC?T... GCD.... SSW.... HCO.... MAO.... DC?T... GGA.... SIS.... HHA.... MGW.... DC?T...";
    HunterView hView = newHunterView(pastPlays, messages);
    int allHistories[NUM_PLAYERS][TRAIL_SIZE];
    PlayerID currentPlayer;
    for (currentPlayer = 0; currentPlayer < NUM_PLAYERS; currentPlayer ++) {
        getHistory(hView, currentPlayer, allHistories[currentPlayer]);
    }
    disposeHunterView(hView);

    int j;
    for (j = 0; j < 7; j ++) {
        printf("%s<--", names[allHistories[PLAYER_DRACULA][j]]);
    }
    
    int numPaths;
    int previousNumPaths;
    int **draculaTrails = getDraculaTrails(allHistories, NULL, &numPaths, 0);
    int **previousTrails = NULL;
    
    printf("\ndepth 0:\n");
    printDraculaTrails(draculaTrails, numPaths);
    
    int maxDepth;
    for (maxDepth = 0; allHistories[PLAYER_DRACULA][maxDepth] != UNKNOWN_LOCATION; maxDepth ++);
    
    int depth;
    for (depth = 1; depth < maxDepth; depth ++) {
        previousTrails = draculaTrails;
        previousNumPaths = numPaths;
        // Use previous dracula trails to incrementally generate more
        draculaTrails = getDraculaTrails(allHistories, previousTrails, &numPaths, depth);
        // Use all possible dracula trails to evaluate a best move

        printf("\ndepth %d:\n", depth);
        int i;
        printf("history...\n");
        for (i = 0; i < depth + 1; i ++) {
            printf("%s<--", names[allHistories[PLAYER_DRACULA][i]]);
        }
        printf("\n");
        printf("numPaths is %d\n", numPaths);
        printDraculaTrails(draculaTrails, numPaths);
        
        // free previous trails
        for (i = 0; i < previousNumPaths; i ++) {
            free(previousTrails[i]);
        }
        free(previousTrails);
    }
    int i;
    for (i = 0; i < numPaths; i ++) {
        free(draculaTrails[i]);
    }
    free(draculaTrails); // trailing trail
    // haha get it, TRAILing trail
    
    return EXIT_SUCCESS;
}


