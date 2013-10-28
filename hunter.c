#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include "Queue.h"
#include "locations.h"

#define TRUE 1
#define FALSE 0

#define MAX_MESSAGE_SIZE 128

#define printf(...)
//#define sqrt 

// When dracula is a in a location, he can travel to a maximum of eight other connected locations
#define MAX_ADJACENT_LOCATIONS 8


void generateMessage(HunterView hView, char *message);
LocationID **getDraculaTrails(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail);
void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths);
int validDraculaTrail(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], int *trail);

static int inArray(LocationID *array, LocationID location, int length);

void decideMove(HunterView hView) {
    PlayerID currentHunter = getCurrentPlayer(hView);
    if (currentHunter < 0 || currentHunter > NUM_PLAYERS - 1) {
        fprintf(stderr, "Calling decideMove on an invalid player %d\n", currentHunter);
        exit(1);
    }
    // backup "default" move for the start
    char bestMove[3] = "JM"; // Start at hospital by defailt
    LocationID curLocation = getLocation(hView, getCurrentPlayer(hView));
    if (curLocation != UNKNOWN_LOCATION) {
        // Otherwise, rest
        strcpy(bestMove, names[getLocation(hView, getCurrentPlayer(hView))]);
    }
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
    int previousNumPaths;
    int **draculaTrails = getDraculaTrails(allHistories, NULL, &numPaths, 0);
    int **previousTrails = NULL;
    getBestMove(hView, bestMove, draculaTrails, numPaths);
    registerBestPlay(bestMove, message);
    
    int depth; // how deep to take the analysis
    int maxDepth;
    if (getRound(hView) < TRAIL_SIZE) {
        maxDepth = getRound(hView);
    } else {
        maxDepth = TRAIL_SIZE;
    }
    for (depth = 1; depth < maxDepth; depth ++) {
        previousTrails = draculaTrails;
        previousNumPaths = numPaths;
        // Use previous dracula trails to incrementally generate more
        draculaTrails = getDraculaTrails(allHistories, previousTrails, &numPaths, depth);
        // Use all possible dracula trails to evaluate a best move
        getBestMove(hView, bestMove, draculaTrails, numPaths);

        // free previous trails
        int i;
        for (i = 0; i < previousNumPaths; i ++) {
            free(previousTrails[i]);
        }
        free(previousTrails);
        // Finally, register best move and message
        registerBestPlay(bestMove, message);
    }
    int i;
    for (i = 0; i < numPaths; i ++) {
        free(draculaTrails[i]);
    }
    free(draculaTrails); // trailing trail
    // haha get it, TRAILing trail
}

double min(double a, double b) {
    if (a < b) {
        return a;
    }
    return b;
}

double max(double a, double b) {
    if (a > b) {
        return a;
    }
    return b;
}

// Determines whether Dracula's trail history contains enough information
// To make it worth analysing
double amountInformation(LocationID trail[TRAIL_SIZE]) {
    // TODO improve this function
    double amountInfo = 0;
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (trail[i] == SEA_UNKNOWN) {
            // Only 10 seas, not much info
            amountInfo += max(0, 1 - (0.1 * i));
        } else if (trail[i] >= ALICANTE && trail[i] <= BLACK_SEA) {
            // We know for sure where he was at some point - definitely worth it
            amountInfo += max(1, 10 - i);
        }
    }
    
    // Wow, such arbitrary
    return amountInfo;
}

void generateMessage(HunterView hView, char *message) {
    if (getRound(hView) == 0) {
        strcpy(message, "mid or feed");
    } else if (getScore(hView) < 100) {
        strcpy(message, "noob team");
    } else if (getScore(hView) < 200) {
        strcpy(message, "you guys suck");
    } else if (getScore(hView) < 300) {
        strcpy(message, "team is failure, uninstall game please");
    } else {
        strcpy(message, "you guys aren't terrible");
    }
}

LocationID **getDraculaTrails(int histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail) {
    // Accepts trails of length n as input, and generates trails of length n + 1 as output
    
    
    // Dracula's travel involves a maximum of 8 adjacent cities for every city
    int numPrevious = *numPaths;
    *numPaths = 0;
    LocationID **generatedTrails;
    
    // Generate all the possible trails
    if (lengthTrail == 0) { // previous paths not relevant
        generatedTrails = malloc(NUM_MAP_LOCATIONS * sizeof(LocationID *)); 
        LocationID currentLocation;
        for (currentLocation = 0; currentLocation < NUM_MAP_LOCATIONS; currentLocation ++) {
            LocationID *initialTrail = malloc(TRAIL_SIZE * sizeof(LocationID));
            initialTrail[0] = currentLocation;
            int i;
            for (i = 1; i < TRAIL_SIZE; i ++) {
                initialTrail[i] = UNKNOWN_LOCATION;
            }
            if (validDraculaTrail(histories, initialTrail)) {
                generatedTrails[*numPaths] = initialTrail;
                *numPaths = *numPaths + 1;
            } else {
                free(initialTrail);
            }
        }
    } else {
        generatedTrails = malloc(MAX_ADJACENT_LOCATIONS * numPrevious * sizeof(LocationID *));
        int pathIndex;
        for (pathIndex = 0; pathIndex < numPrevious; pathIndex ++) {
            LocationID lastCity = previousPaths[pathIndex][lengthTrail - 1];

            int newIndex = 0;
            // add all possible land moves
            for (newIndex = 0; adjacencyRoad[lastCity][newIndex] != END; newIndex ++) {
                LocationID *newPath = malloc(TRAIL_SIZE * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                if (adjacencyRoad[lastCity][newIndex] > NUM_MAP_LOCATIONS || adjacencyRoad[lastCity][newIndex] < 0) {
                    printf("SOMETHING SERIOUSLY WRONG\n");
                    getchar();
                }
                assert(lengthTrail < TRAIL_SIZE);
                newPath[lengthTrail] = adjacencyRoad[lastCity][newIndex];

                if (validDraculaTrail(histories, newPath)) {
                    int k;
                    for (k = 0; k < TRAIL_SIZE; k++) {
                        assert(newPath[k] < NUM_LOCATIONS);
                    }
                    generatedTrails[*numPaths] = newPath;
                    *numPaths = *numPaths + 1;
                } else {
                    free(newPath);
                }
            }
            // add all possible sea moves
            for (newIndex = 0; adjacencySea[lastCity][newIndex] != END; newIndex ++) {
                LocationID *newPath = malloc(TRAIL_SIZE * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = adjacencySea[lastCity][newIndex];
                if (validDraculaTrail(histories, newPath)) {
                    generatedTrails[*numPaths] = newPath;
                    *numPaths = *numPaths + 1;
                } else {
                    free(newPath);
                }
            }
            // special move: teleport
            if (histories[PLAYER_DRACULA][lengthTrail] == TELEPORT) {
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
        if (trail[i] == UNKNOWN_LOCATION || histories[PLAYER_DRACULA][i] == UNKNOWN_LOCATION) {
            break;
        }
        if (trail[i] == ST_JOSEPH_AND_ST_MARYS) {
            return FALSE;
        }

        // check that any double backs or hides match, otherwise
        // check that city is not in trail
        if (histories[PLAYER_DRACULA][i] == HIDE) {
            if (i < TRAIL_SIZE - 1 && trail[i] != trail[i + 1]) {
                return FALSE;
            } else if (trail[i + 1] >= NORTH_SEA && trail[i + 1] <= BLACK_SEA) {
                // dracula cannot hide at sea
                return FALSE;
            }
        } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_1) {
            if (i < TRAIL_SIZE - 1 && trail[i] != trail[i + 1]) {
                return FALSE;
            }
        } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_2) {
            if (i < TRAIL_SIZE - 2 && trail[i] != trail[i + 2]) {
                return FALSE;
            }
        } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_3) {
            if (i < TRAIL_SIZE - 3 && trail[i] != trail[i + 3]) {
                return FALSE;
            }
        } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_4) {
            if (i < TRAIL_SIZE - 4 && trail[i] != trail[i + 4]) {
                return FALSE;
            }
        } else if (histories[PLAYER_DRACULA][i] == DOUBLE_BACK_5) {
            if (i < TRAIL_SIZE - 5 && trail[i] != trail[i + 5]) {
                return FALSE;
            }
        } else if (histories[PLAYER_DRACULA][i] == TELEPORT) {
            if (trail[i] != CASTLE_DRACULA) {
                return FALSE;
            }
        } else {
            // check that the city is not in 6 range
            int j;
            for (j = i + 1; j < 6; j ++) {
                if (trail[i] == trail[j]) {
                    return FALSE;
                }
            }
        }

        
        // Check that dracula history matches the location in trail
        if (histories[PLAYER_DRACULA][i] < NUM_MAP_LOCATIONS && histories[PLAYER_DRACULA][i] != trail[i]) {
            return FALSE;
        } else if (histories[PLAYER_DRACULA][i] == CITY_UNKNOWN && trail[i] > ZURICH) {
            return FALSE;
        } else if (histories[PLAYER_DRACULA][i] == SEA_UNKNOWN && (trail[i] < NORTH_SEA || trail[i] > BLACK_SEA)) {
            return FALSE;
        } else if (trail[i] == ST_JOSEPH_AND_ST_MARYS) { // dracula is banned from church since he is atheist
            return FALSE;
        }

        // check that the trail matches the locations of the hunters]
        if (histories[PLAYER_DRACULA][i] == CITY_UNKNOWN) {
        PlayerID currentPlayer;
            for (currentPlayer = 0; currentPlayer < NUM_PLAYERS - 1; currentPlayer ++) {
                // cannot be city unknown if we were in that city
                if (trail[i] == histories[currentPlayer][i]) {
                    return FALSE;
                }
            }
        }
    }
    
    return TRUE;
}


static int inArray(LocationID *array, LocationID location, int length) {
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

// BFS and find the distance from a point to every other point
void findDistances(HunterView hView, LocationID source, PlayerID player, int distance[NUM_MAP_LOCATIONS]) {
    int i;

    int seen[NUM_MAP_LOCATIONS];
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        seen[i] = FALSE;
    }
    Round startRound = getRound(hView);

    Queue q = QueueCreate();
    QueuePush(q, source, source, startRound);
    while (!QueueEmpty(q)) {
        queueData data = QueuePop(q);
        if (seen[data.location]) {
            continue;
        }
        seen[data.location] = TRUE;
        distance[data.location] = data.round - startRound;

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
}

void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths) {
    // Uses the relevant trail data to predict where dracula could move next, and have the
    // hunter move accordingly

    PlayerID player = getCurrentPlayer(hView);
    LocationID playerLoc = getLocation(hView, player);
    // If we don't have a location yet
    if (playerLoc == UNKNOWN_LOCATION) {
        if (player == PLAYER_LORD_GODALMING) {
            strcpy(bestMove, names[ST_JOSEPH_AND_ST_MARYS]);
        } else if (player == PLAYER_DR_SEWARD) {
            strcpy(bestMove, names[HAMBURG]);
        } else if (player == PLAYER_VAN_HELSING) {
            strcpy(bestMove, names[GENOA]);
        } else if (player == PLAYER_MINA_HARKER) {
            strcpy(bestMove, names[TOULOUSE]);
        }
        return;
    }

    int isInCastleDracula = FALSE;

    // Store all the current state information
    Round curRound = getRound(hView);
    LocationID myHistory[TRAIL_SIZE];
    getHistory(hView, player, myHistory);
    LocationID draculaTrail[TRAIL_SIZE];
    getHistory(hView, PLAYER_DRACULA, draculaTrail);
    

    int i;
    /*// First see if we should research if not enough info
    LocationID draculaTrail[TRAIL_SIZE];
    
    int trailSize = 0;
    int numUnknown = 0;
    getHistory(hView, PLAYER_DRACULA, draculaTrail);
    
    if (draculaTrail[0] == CASTLE_DRACULA) {
        isInCastleDracula = TRUE;
    }
    
    for (i = 0; i < 8; i++) {
        trailSize++;
        if (draculaTrail[i] == UNKNOWN_LOCATION) {
            break;
        }

        if (draculaTrail[i] == CITY_UNKNOWN || draculaTrail[i] == SEA_UNKNOWN) {
            numUnknown++;
        }
    }
    if (numUnknown == 8) {
        printf("8 unknown, rest and research\n");
        // So little information, rest and research
        strcpy(bestMove, names[playerLoc]);
        return;
    }
    if (trailSize <= 6 && numUnknown >= 6) {
        printf("%d unknown at start, rest and research\n", numUnknown);
        // no info, rest and research
        strcpy(bestMove, names[playerLoc]);
        return;
    }*/
    

    
    // Create a probability array of possible locations Dracula could be in this turn
    int probableNow[NUM_MAP_LOCATIONS];
    // Create a probability array of possible locations Dracula could be in next turn
    int probableNext[NUM_MAP_LOCATIONS];
    // Distance from each hunter
    int distance[NUM_PLAYERS - 1][NUM_MAP_LOCATIONS];
    double score[NUM_MAP_LOCATIONS];
    for (i = 0; i < NUM_PLAYERS - 1; i++) {
        findDistances(hView, getLocation(hView, i), i, distance[i]);
    }
    // TODO pass frequency...
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        probableNow[i] = 0;
        probableNext[i] = 0;
        score[i] = 0;
    }
    
    /*printf("Player %d at %s, distances:\n", player, names[playerLoc]);
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        printf("%s: %d\n", names[i], distance[1][i]);
    }
    printf("\n");*/
    
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
    
    int draculaAtSea = FALSE;
    if (draculaTrail[0] == SEA_UNKNOWN) {
        draculaAtSea = TRUE;
    } else if (draculaTrail[0] >= NORTH_SEA && draculaTrail[0] <= BLACK_SEA) {
        draculaAtSea = draculaTrail[0];
    }

    LocationID location;
    printf("%d possible paths\n", numPaths);
    for (location = 0; location < NUM_MAP_LOCATIONS; location++) {        
        // Probabilities
        double pHere = (double)probableNow[location] / numPaths;
        double pNext = (double)probableNext[location] / numPaths;
        
        double curScore = (10 * pHere) + (2 * pNext);
        
        score[location] = curScore;
    }
    double highScore[5] = {0, 0, 0, 0, 0};
    int highScoreID[5];
    for (i = 0; i < 5; i++) {
        int k;
        for (k = 0; k < NUM_MAP_LOCATIONS; k++) {
            int j;
            int skipThis = FALSE;
            for (j = 0; j < i; j++) {
                if (k == highScoreID[j]) {
                    skipThis = TRUE;
                }
            }
            if (skipThis) {
                continue;
            }
            if (score[k] > highScore[i]) {
                highScore[i] = score[k];
                highScoreID[i] = k;
            }
        }
    }
    printf("Top 5 scored locations:\n");
    for (i = 0; i < 5; i++) {
        printf("#%d: %lf(%s)\n", i+1, highScore[i], names[highScoreID[i]]);
    }
    
    // spreadCoeff should be between 0.5 and 1.8
    double amountOfInfo = amountInformation(draculaTrail);
    printf("amountOfInfo = %lf\n", amountOfInfo);
    double spreadCoeff = 1.8;
    if (amountOfInfo > 0.1) {
        spreadCoeff = (double)5 / amountOfInfo;
        spreadCoeff = min(1.8, spreadCoeff);
        spreadCoeff = max(0.5, spreadCoeff);
    }
    printf("spreadCoeff = %lf\n", spreadCoeff);

    double bestMoveScore = 0;
    LocationID moveLocation = playerLoc;
    for (i = 0; i < numAdjLocs; i++) {
        LocationID curLocation = adjLocs[i];
        if (draculaAtSea == TRUE) {
            // If dracula is at some unknown sea, don't take sea
            continue;
        }
        int k;
        double distToPlayers = 0;
        for (k = 0; k < NUM_PLAYERS - 1; k++) {
            if (k != player) {
                distToPlayers += distance[k][curLocation];
            }
        }
        double distScore = sqrt(distToPlayers);
        
        double locationScore = 0;
        for (k = 0; k < NUM_MAP_LOCATIONS; k++) {
            LocationID *path = NULL;
            locationScore += score[k] / sqrt(shortestPath(hView, adjLocs[i], k, &path));
            free(path);
        }
        
        double alreadyExplored = 0;
        for (k = 0; k < TRAIL_SIZE; k++) {
            if (curLocation == myHistory[k]) {
                alreadyExplored = 1 - ((double)k*0.2);
                break;
            }
        }
        
        double curScore = spreadCoeff * distScore + locationScore - alreadyExplored;
        if (curScore > bestMoveScore) {
            bestMoveScore = curScore;
            moveLocation = adjLocs[i];
        }
        
        printf("Score of moving to %s is %lf (%lf + %lf - %lf)\n", names[adjLocs[i]], curScore, spreadCoeff * distScore, locationScore, alreadyExplored);
    }
            
    free(adjLocs);
    strcpy(bestMove, names[moveLocation]);
    
    if (isInCastleDracula) {
        fprintf(stdout, "Dracula is in Castle Dracula and destination is %d(%s)\n", moveLocation, names[moveLocation]);
    }
}
