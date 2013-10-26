#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
#include "locations.h"

#define printf(...)

/*
static LocationID moveID(char a, char b) {
    char move[3];
    move[0] = a;
    move[1] = b;
    move[2] = '\0';
    
    // credit to Claudia Tu and Daniel Li for this array
    char locations[NUM_LOCATIONS][3] = {
        // 61 cities
        "AL", "AM", "AT", "BA", "BI", "BE", "BR", "BO", "BU", "BC",
        "BD", "CA", "CG", "CD", "CF", "CO", "CN", "DU", "ED", "FL",
        "FR", "GA", "GW", "GE", "GO", "GR", "HA", "JM", "KL", "LE",
        "LI", "LS", "LV", "LO", "MA", "MN", "MR", "MI", "MU", "NA",
        "NP", "NU", "PA", "PL", "PR", "RO", "SA", "SN", "SR", "SJ",
        "SO", "ST", "SW", "SZ", "TO", "VA", "VR", "VE", "VI", "ZA",
        "ZU",
        // 10 seas
        "NS", "EC", "IS", "AO", "BB", "MS", "TS", "IO", "AS", "BS",
        // misc
        "C?", "S?", "HI", "D1", "D2", "D3", "D4", "D5", "TP"
    };
    
    int i;
    for (i = 0; i < NUM_LOCATIONS; i++) {
        if (strcmp(move, locations[i]) == 0) {
            // strings are equal, ID found
            return i;
        }
    }
    
    // move not found in array
    fprintf(stderr, "LocationID not found for move %s\n", move);
    exit(1);
}
*/

int genRand(int start, int end) {
   double baseRand = ((double)rand())/RAND_MAX;
   int range = end - start + 1;

   return (int)(baseRand*range) + start;
}

int inArray(LocationID *array, LocationID location, int length) {
    int i;
    for (i = 0; i < length; i++) {
        if (array[i] == location) {
            return TRUE;
        }
    }
    return FALSE;
}

void decideMoveDracula (GameView g) {
    printf("Dracula: YAY it's my turn!\n");
    LocationID curLocation = gameGetLocation(g, PLAYER_DRACULA);
    printf("Dracula: I am at %d(%s)\n", curLocation, names[curLocation]);
    if (curLocation == UNKNOWN_LOCATION) {
        //printf("Dracula: I am lost =[ I go home to Castle Dracula\n");
        printf("Dracula: I am lost =[ I go to Madrid\n");
        curLocation = MADRID;
    }
    int numAdjLocs;
    LocationID *adjLocs = connectedLocations(gameGetHunterView(g), &numAdjLocs, curLocation, PLAYER_DRACULA,
                                             0, TRUE, FALSE, TRUE);
    LocationID trail[TRAIL_SIZE];
    gameGetHistory(g, PLAYER_DRACULA, trail);
    
    // Teleport if no legit move
    int validMove = FALSE;
    int k;
    for (k = 0; k < numAdjLocs; k++) {
        if (!inArray(trail, adjLocs[k], TRAIL_SIZE)) {
            validMove = TRUE;
        }
    }
    if (!validMove) {
        printf("Dracula: There is actually no valid move! Teleporting...\n");
        registerBestPlay("TP", "");
        return;
    }
    
    int direction = genRand(1, numAdjLocs - 1);
    printf("Dracula: Rolled a %d from %d to %d\n", direction, 1, numAdjLocs);
    printf("Dracula: I have decided to go to %s!\n", names[adjLocs[direction]]);
    printf("Dracula: Don't forget my trail is: ");
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        printf("%d(%s) ", trail[i], names[trail[i]]);
    }
    printf("\n");
    
    while (inArray(trail, adjLocs[direction], TRAIL_SIZE) && direction < numAdjLocs) {
        printf("Dracula: oh, I can't go there...ok then =[\n");
        direction = genRand(1, numAdjLocs - 1);
    }
    char bestMove[3];
    if (direction >= numAdjLocs) {
        printf("Dracula: gg can't go anywhere not moving anymore\n");
        strcpy(bestMove, names[curLocation]);
    } else {
        strcpy(bestMove, names[adjLocs[direction]]);
    }
    
    printf("Dracula: I will go to %s\n", bestMove);
    registerBestPlay(bestMove, "");
    free(adjLocs);
}
