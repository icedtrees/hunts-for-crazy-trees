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
void getBestMove(HunterView hView, char *bestMove, char **draculaPaths, int numPaths);

static int inPath(char *path, LocationID location);


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

static int inPath(char *path, LocationID location) {
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (path[i] == location) {
            return TRUE;
        }
    }
    return FALSE;
}

// Recursively go through backtrace and create an array of the path
// Returns the length of the path
static int rPush(LocationID source, LocationID curLoc, LocationID backtrace[], LocationID *path, int curDistance) {
    if (curLoc == source) {
        path = malloc(curDistance * sizeof(LocationID));
        path[0] = source;
        return 1;
    }
    int len = rPush(source, backtrace[curLoc], backtrace, path, curDistance + 1) + 1;
    path[len-1] = curLoc;

    return len;
}

// Returns distance of path and array containing path by reference
// Returns -1 if no path found
static int shortestPath(HunterView hView, LocationID source, LocationID dest, LocationID *path) {
    int found = FALSE;
    int i;

    int seen[NUM_MAP_LOCATIONS];
    LocationID backtrace[NUM_MAP_LOCATIONS];
    for (i = 0; i < NUM_MAP_LOCATIONS) {
        seen[i] = FALSE;
        backtrace[i] = -1;
    }

    Queue q = QueueCreate();
    QueuePush(q, QueueCreateNode(source, source));
    while (!QueueEmpty(q)) {
        QueueNode curNode = QueuePop(q);
        if (seen[curNode.location]) {
            continue;
        }
        seen[curNode.location] = TRUE;
        backtrace[curNode.location] = curNode.from;
        if (curNode.location == dest) {
            found = TRUE;
            break;
        }

        int numAdjLocs;
        LocationID *adjLocs = connectedLocations(hView, &numAdjLocs, curLoc, PLAYER_DRACULA,
                                                 curRound, TRUE, FALSE, TRUE);
        for (i = 0; i < numAdjLocs; i++) {
            if (!seen[adjLocs[i]]) {
                QueuePush(q, QueueCreateNode(adjLocs[i], curNode.location));
            }
        }
    }

    if (found) {
        return rPush(src, dest, backtrace, path, 1);
    } else {
        return -1;
    }
}

void getBestMove(HunterView hView, char *bestMove, char **draculaPaths, int numPaths) {
    // Uses the relevant trail data to predict where dracula could move next, and have the
    // hunter move accordingly

    // First create a TRUE/FALSE array of possible locations Dracula could be in next turn
    LocationID possible[NUM_MAP_LOCATIONS];
    memset(possible, 0, NUM_MAP_LOCATIONS * sizeof(LocationID));

    // Store all the current state information
    PlayerID player = getCurrentPlayer(hView);
    Round curRound = getRound(hView);

    // Begin filling in the possible locations array
    int i;
    for (i = 0; i < numPaths; i++) {
        LocationID curLoc = draculaPaths[i][0];
        int numAdjLocs;
        LocationID *adjLocs = connectedLocations(hView, &numAdjLocs, curLoc, PLAYER_DRACULA,
                                                 curRound, TRUE, FALSE, TRUE);
        
        // For each adjacent location that he could've moved to,
        // increase the corresponding possible array element
        // if it doesn't cross back into his trail
        int j;
        for (j = 0; j < numAdjLocs; j++) {
            // TODO check if dracula has doubled back already
            // and if not, he might be able to double back into his path
            if (!inPath(draculaPaths[i], adjLoc[j])) {
                possible[adjLocs[j]]++;
            }
        }

        // Don't forget to free adjLocs
        free(adjLocs);
    }

    // TODO don't use this strat, improve to a better one
    // that takes into account distances etc.
    // Find the most likely location Dracula is at
    LocationID mostLikely = 0;
    int highestProb = 0;
    for (i = 0; i < NUM_MAP_LOCATIONS) {
        if (possible[i] > highestProb) {
            highestProb = possible[i];
            mostLikely = i;
        }
    }

    // Get the first step of the optimal path towards our destination
    LocationID *pathToTake;
    int pathLength = shortestPath(hView, getLocation(hView, player), mostLikely, pathToTake);
    LocationID firstStep = pathToTake[0];
    free(pathToTake);

    return firstStep;
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
