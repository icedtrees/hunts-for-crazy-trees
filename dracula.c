#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
#include "locations.h"
#include "Queue.h"

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


static int rPush(LocationID source, LocationID curLoc, LocationID backtrace[], LocationID **path, int curDistance) {
    if (curLoc == source) {
        *path = malloc(curDistance * sizeof(LocationID));
        (*path)[0] = source;
        return 1;
    }
    int len = rPush(source, backtrace[curLoc], backtrace, path, curDistance + 1) + 1;
    (*path)[len-1] = curLoc;
    
    return len;
}


// Returns distance of path and array containing path by reference
// Returns -1 if no path found
static int shortestPathNextTurn(HunterView hView, LocationID source, LocationID dest, LocationID **path) {
    int found = FALSE;
    int i;

    int seen[NUM_MAP_LOCATIONS];
    LocationID backtrace[NUM_MAP_LOCATIONS];
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        seen[i] = FALSE;
        backtrace[i] = -1;
    }
    Round startRound = getRound(hView);
    PlayerID player = getCurrentPlayer(hView);

    Queue q = QueueCreate();
    QueuePush(q, source, source, startRound);
    while (!QueueEmpty(q)) {
        queueData data = QueuePop(q);
        if (seen[data.location]) {
            continue;
        }
        seen[data.location] = TRUE;
        backtrace[data.location] = data.from;
        if (data.location == dest) {
            found = TRUE;
            break;
        }

        int numAdjLocs;
        LocationID *adjLocs = connectedLocations(hView, &numAdjLocs, data.location, player,
                                                 data.round + 1, TRUE, TRUE, TRUE);
        for (i = 0; i < numAdjLocs; i++) {
            if (!seen[adjLocs[i]]) {
                QueuePush(q, adjLocs[i], data.location, data.round + 2);
            }
        }
        free(adjLocs);
    }
    QueueDispose(q);
    if (found) {
        int temp = rPush(source, dest, backtrace, path, 1);
        return temp;
    } else {
        return -1;
    }
}

LocationID furthestValidCity(GameView g, LocationID *trail) {
    // Returns the non-sea location furthest from any hunter    
    HunterView hView = gameGetHunterView(g);
    int numConnected;
    LocationID *validCities = connectedLocations(hView, &numConnected, gameGetLocation(g, PLAYER_DRACULA), PLAYER_DRACULA, getRound(hView), TRUE, FALSE, TRUE);
    int currentMaxDistance = -1;
    int currentBestCity = UNKNOWN_LOCATION;
    int i;
    for (i = 0; i < numConnected; i ++) {
        int minHunterDistance = 100; // large
        int hunter;
        for (hunter = 0; hunter < NUM_PLAYERS - 1; hunter ++) {
            LocationID *path = NULL;
            int distance = shortestPathNextTurn(hView, gameGetLocation(g, hunter), validCities[i], &path);
            if (distance < minHunterDistance) {
                minHunterDistance = distance;
            }
        }
        if (minHunterDistance > currentMaxDistance && !inArray(trail, validCities[i], 6)) {
            currentMaxDistance = minHunterDistance;
            currentBestCity = validCities[i];
        }
    }
    return currentBestCity;
}

void decideMoveDracula (GameView g) {
    printf("Dracula: YAY it's my turn!\n");
    LocationID curLocation = gameGetLocation(g, PLAYER_DRACULA);
    printf("Dracula: I am at %d(%s)\n", curLocation, names[curLocation]);
    if (curLocation == UNKNOWN_LOCATION) {
        //printf("Dracula: I am lost =[ I go home to Castle Dracula\n");
        printf("Dracula: I am lost =[ I go to Vienna\n");
        curLocation = VIENNA;
        registerBestPlay("VI", "");
        return;
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



    char bestMove[3];    
    LocationID furthest = furthestValidCity(g, trail);
    if (furthest != UNKNOWN_LOCATION) {
        strcpy(bestMove, names[furthest]);
    } else {
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
        if (direction >= numAdjLocs) {
            printf("Dracula: gg can't go anywhere not moving anymore\n");
            strcpy(bestMove, names[curLocation]);
        } else {
            strcpy(bestMove, names[adjLocs[direction]]);
        }
    }
    
    printf("Dracula: I will go to %s\n", bestMove);
    registerBestPlay(bestMove, "");
    free(adjLocs);
}
