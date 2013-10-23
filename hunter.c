#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include "Queue.h"
#include "locations.h"

#define TRUE 1
#define FALSE 0

#define MAX_MESSAGE_SIZE 128

// When dracula is a in a location, he can travel to a maximum of eight other connected locations
#define MAX_ADJACENT_LOCATIONS 8



static const int primes[26] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 73, 79, 83, 89, 97, 101, 103};
static const int id[323] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 55, -1, -1, 3, -1, 33, -1, -1, -1, 52, -1, 11, -1, -1, -1, -1, -1, -1, 65, -1, -1, 57, -1, 19, -1, -1, -1, -1, -1, -1, -1, 5, -1, -1, -1, -1, -1, 32, -1, 37, -1, -1, -1, -1, 60, -1, 56, -1, -1, 45, -1, -1, -1, -1, 15, -1, 38, -1, -1, 21, 50, -1, 31, -1, -1, -1, 9, 26, -1, -1, -1, 54, -1, 17, -1, -1, 24, -1, -1, -1, 59, -1, -1, -1, -1, -1, -1, 68, -1, -1, -1, -1, -1, -1, -1, -1, 36, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, -1, -1, -1, -1, -1, -1, 28, -1, 51, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 49, -1, 10, -1, -1, -1, -1, -1, 12, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, 34, -1, -1, 7, -1, -1, -1, -1, 39, -1, 47, 67, -1, -1, -1, -1, -1, 6, -1, 44, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, -1, -1, 61, -1, -1, 14, -1, 16, -1, 53, -1, 43, -1, -1, -1, -1, 63, -1, 42, -1, -1, -1, 18, -1, -1, 64, -1, -1, -1, -1, 70, -1, -1, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, 20, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 69, -1, -1, -1, -1, 66, -1, -1, -1, -1, -1, 46, -1, -1, -1, 25, 41, 48, 62, -1, -1, 29, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 22, -1, -1, -1, -1, -1, 4, -1, -1, -1, 27, -1, -1, 58, -1, -1, -1, -1, -1, 40, -1, -1, -1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2};



void generateMessage(HunterView hView, char *message);
LocationID **getDraculaTrails(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail);
void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths);
int validDraculaTrail(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], int *trail);
LocationID cityID(char name[3]);
int intPow(int base, int index);

int inArray(LocationID *array, LocationID location, int length);

void decideMove(HunterView hView) {
    // backup "default" move for the start
    // rest
    char bestMove[3];
    strcpy(bestMove, names[getLocation(hView, getCurrentPlayer(hView))]);
    
    // BM the crap out of other hunters
    char message[MAX_MESSAGE_SIZE];
    generateMessage(hView, message);
    
    registerBestPlay(bestMove, message);
    
    
    // Initialise all the histories for all the players
    int allHistories[NUM_PLAYERS][TRAIL_SIZE];
    PlayerID currentPlayer;
    for (currentPlayer = 0; currentPlayer < NUM_PLAYERS; currentPlayer ++) {
        getHistory(hView, currentPlayer, allHistories[currentPlayer]);
    }
    
    // Begin analysing the information we have, incrementally analysing deeper
    // Get initial trails of length 0 (1 city)
    int numPaths;
    int **draculaTrails = getDraculaTrails(allHistories, NULL, &numPaths, 0);
    int **previousTrails = NULL;
    
    getBestMove(hView, bestMove, draculaTrails, numPaths);
    registerBestPlay(bestMove, message);
    
    
    int depth; // how deep to take the analysis
    int maxDepth;
    if (getRound(hView) < 6) {
        maxDepth = getRound(hView);
    } else {
        maxDepth = 6;
    }
    for (depth = 1; depth < maxDepth; depth ++) {
        previousTrails = draculaTrails;
    
        // Use previous dracula trails to incrementally generate more
        draculaTrails = getDraculaTrails(allHistories, previousTrails, &numPaths, depth);
        
        
        // Use all possible dracula trails to evaluate a best move
        getBestMove(hView, bestMove, draculaTrails, numPaths);
        
        
        // free previous trails
        int i;
        for (i = 0; i < numPaths; i ++) {
            free(previousTrails[i]);
        }
        free(previousTrails);      
          
        // Finally, register best move and message
        registerBestPlay(bestMove, message);
    }
}

// Determines whether Dracula's trail history contains enough information
// To make it worth analysing
int enoughInformation(LocationID trail[TRAIL_SIZE]) {
    // TODO improve this function
    int amountInfo = 0;
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (trail[i] == CITY_UNKNOWN) {
            // Useless (almost)
            amountInfo += 1;
        } else if (trail[i] == SEA_UNKNOWN) {
            // Only 10 seas, pretty good info
            amountInfo += 5;
        } else if (trail[i] >= ALICANTE && trail[i] <= BLACK_SEA) {
            // We know for sure where he was at some point - definitely worth it
            amountInfo += 100;
        }
    }
    
    // Wow, such arbitrary
    return amountInfo > 10;
}

void generateMessage(HunterView hView, char *message) {
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

LocationID **getDraculaTrails(int histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail) {
    // Accepts trails of length n as input, and generates trails of length n + 1 as output
    
    
    // Dracula's travel involves a maximum of 8 adjacent cities for every city
    LocationID **generatedTrails = malloc(NUM_MAP_LOCATIONS * intPow(MAX_ADJACENT_LOCATIONS, lengthTrail) * sizeof(int *));
    int numPrevious = *numPaths;
    *numPaths = 0;
    
    
    // Generate all the possible trails
    if (lengthTrail == 0) { // previous paths not relevant
        LocationID currentLocation;
        for (currentLocation = 0; currentLocation < NUM_MAP_LOCATIONS; currentLocation ++) {
            LocationID *initialTrail = malloc(TRAIL_SIZE * sizeof(int));
            initialTrail[0] = currentLocation;
            int i;
            for (i = 1; i < TRAIL_SIZE; i ++) {
                initialTrail[i] = -1;
            }
            if (validDraculaTrail(histories, initialTrail)) {
                generatedTrails[*numPaths] = initialTrail;
                *numPaths = *numPaths + 1;
            }
        }
    } else {
        int pathIndex;
        for (pathIndex = 0; pathIndex < numPrevious; pathIndex ++) {
            LocationID lastCity = previousPaths[pathIndex][lengthTrail - 1];
            int newIndex;
            // add all possible land moves
            for (newIndex = 0; adjacencyRoad[lastCity][newIndex] != END; newIndex ++) {
                LocationID *newPath = malloc(TRAIL_SIZE * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = adjacencyRoad[lastCity][newIndex];

                if (validDraculaTrail(histories, newPath)) {
                    generatedTrails[*numPaths] = newPath;
                    *numPaths = *numPaths + 1;
                } else {
                    free(newPath);
                }
            }
            // add all possible sea moves
            for (newIndex = 0; adjacencySea[lastCity][newIndex] != END; newIndex ++) {
                LocationID *newPath = malloc((lengthTrail + 1) * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = adjacencyRoad[lastCity][newIndex];

                if (validDraculaTrail(histories, newPath)) {
                    generatedTrails[*numPaths] = newPath;
                    *numPaths = *numPaths + 1;
                } else {
                    free(newPath);
                }
            }
            // special move: teleport
            if (histories[PLAYER_DRACULA][0] == TELEPORT) {
                LocationID *newPath = malloc(TRAIL_SIZE * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = CASTLE_DRACULA;
                generatedTrails[*numPaths] = newPath;
                *numPaths = *numPaths + 1;
            }
        }
    }

    return generatedTrails;
}

int validDraculaTrail(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], int *trail) {
    // Given a dracula trail where all cities are adjacent, verifies that it matches histories
    // and all double backs/hides are legitimate
    int i;
    for (i = 0; i < TRAIL_SIZE; i ++) {
        // Iterate through the trail dracula has made, check all locations/moves are valid
        /*
        if (trail[i + 1] != -1) {
            // check that city is not in trail (implement later)
            if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_1) {
            } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_2) {
            } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_3) {
            } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_4) {
            } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_5) {
            } else {
            }
        }
        */
        
        // Check that dracula history matches the location in trail
        if (histories[PLAYER_DRACULA][i] < NUM_MAP_LOCATIONS && histories[PLAYER_DRACULA][i] != trail[i]) {
            return FALSE;
        } else if (histories[PLAYER_DRACULA][i] == CITY_UNKNOWN && trail[i] > ZURICH) {
            return FALSE;
        } else if (histories[PLAYER_DRACULA][i] == SEA_UNKNOWN && (trail[i] < NORTH_SEA || trail[i] > BLACK_SEA)) {
            return FALSE;
        }

        // check that the trail matches the locations of the hunters
        // do later
    }
    
    return TRUE;
}

LocationID cityID(char name[3]) {
    // Hash function requires 323 bytes of memory; is injective
    return id[(primes[name[0] - 'A'] * primes[name[1] - 'A'] * primes[name[1] - 'A']) % 323];
}

int intPow(int base, int index) {
    if (index == 0) {
        return 1;
    }
    return base * intPow(base, index - 1);
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

// Recursively go through backtrace and create an array of the path
// Returns the length of the path
int rPush(LocationID source, LocationID curLoc, LocationID backtrace[], LocationID **path, int curDistance) {
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
int shortestPath(HunterView hView, LocationID source, LocationID dest, LocationID **path) {
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
                                                 data.round, TRUE, TRUE, TRUE);
        for (i = 0; i < numAdjLocs; i++) {
            if (!seen[adjLocs[i]]) {
                QueuePush(q, adjLocs[i], data.location, data.round + 1);
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

void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths) {
    // Uses the relevant trail data to predict where dracula could move next, and have the
    // hunter move accordingly
    
    // Store all the current state information
    PlayerID player = getCurrentPlayer(hView);
    LocationID playerLoc = getLocation(hView, player);
    Round curRound = getRound(hView);
    
    // If we don't have a location yet
    if (playerLoc == UNKNOWN_LOCATION) {
        strcpy(bestMove, names[ST_JOSEPH_AND_ST_MARYS]);
        return;
    }

    // Create a probability array of possible locations Dracula could be in this turn
    int probableNow[NUM_MAP_LOCATIONS];
    // Create a probability array of possible locations Dracula could be in next turn
    int probableNext[NUM_MAP_LOCATIONS];
    // TODO also have array for distance from other hunters
    // TODO also have array for paths that pass through location
    int i;
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        probableNow[i] = 0;
        probableNext[i] = 0;
    }
    
    // Begin filling in the probableNow locations array
    for (i = 0; i < numPaths; i++) {
        probableNow[draculaPaths[i][0]]++;
    }
    
    // Begin filling in the probableNext locations array
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
            if (!inArray(draculaPaths[i], adjLocs[j], TRAIL_SIZE)) {
                probableNext[adjLocs[j]]++;
            }
        }
        // Don't forget to free adjLocs
        free(adjLocs);
    }
    
    // TODO don't use this strat, improve to a better one
    // that takes into account distances etc.
    // Find the most likely location Dracula is at
    int numAdjLocs;
    LocationID *adjLocs = connectedLocations(hView, &numAdjLocs, playerLoc, player,
                                             curRound, TRUE, TRUE, TRUE);
    LocationID destination = 0;
    int highScore = 0;
    LocationID location;
    for (location = 0; location < NUM_MAP_LOCATIONS; location++) {
        int curScore = 0;
        // If we are next to the location being considered, high chance
        // that we want to investigate it if there's a chance he's right there
        if (inArray(adjLocs, location, numAdjLocs)) {
            curScore += 5 * probableNow[location];
        }
        curScore += probableNext[location];
        if (curScore > highScore) {
            highScore = curScore;
            destination = location;
        }
    }
    free(adjLocs);
    
    // Get the first step of the optimal path towards our destination
    LocationID firstStep = playerLoc; // Default move
    if (playerLoc != destination) {
        LocationID *pathToTake = NULL;
        int length = shortestPath(hView, playerLoc, destination, &pathToTake);
        assert(length > 1);
        if (pathToTake != NULL) {
            firstStep = pathToTake[1];
            free(pathToTake);
        }
    
        /*printf("Best place to go is %s\n", names[destination]);
        printf("Path is: ");
        for (i = 0; i < length; i++) {
            printf("%s ", names[pathToTake[i]]);
        }
        printf("\n");*/
    }
    
    strcpy(bestMove, names[firstStep]);
}
