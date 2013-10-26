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

#define printf(...)

// When dracula is a in a location, he can travel to a maximum of eight other connected locations
#define MAX_ADJACENT_LOCATIONS 8


void generateMessage(HunterView hView, char *message);
LocationID **getDraculaTrails(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail);
void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths);
int validDraculaTrail(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], int *trail);
int intPow(int base, int index);

static int inArray(LocationID *array, LocationID location, int length);

void decideMove(HunterView hView) {
    PlayerID currentHunter = getCurrentPlayer(hView);
    if (currentHunter < 0 || currentHunter > NUM_PLAYERS - 1) {
        fprintf(stderr, "Calling decideMove on an invalid player %d\n", currentHunter);
        exit(1);
    }
    // backup "default" move for the start
    char bestMove[3] = "JM"; // Start at hospital
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
    if (getRound(hView) < 6) {
        maxDepth = getRound(hView);
    } else {
        maxDepth = 6;
    }
    printf("depth = 0: draculaTrails is %p, numPaths is %d\n", draculaTrails, numPaths);
    for (depth = 1; depth < maxDepth; depth ++) {
        printf("start of loop, depth = %d, draculaTrails is %p, numPaths is %d\n", depth, draculaTrails, numPaths);
        printf("previousTrails = %p, previousNumPaths = %d\n", previousTrails, previousNumPaths);
        printf("assigning previousTrails and previousNumPaths...\n");
        previousTrails = draculaTrails;
        previousNumPaths = numPaths;
        printf("previousTrails is now %p, previousNumPaths is now %d\n", previousTrails, previousNumPaths);
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
    free(draculaTrails); // trailing trail
    // haha get it, TRAILing trail
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
    return amountInfo > 12;
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
    LocationID **generatedTrails = malloc(NUM_MAP_LOCATIONS * intPow(MAX_ADJACENT_LOCATIONS, lengthTrail) * sizeof(LocationID *));
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
            } else {
                free(initialTrail);
            }
        }
    } else {
        int pathIndex;
        for (pathIndex = 0; pathIndex < numPrevious; pathIndex ++) {
            fflush(stdout);
            LocationID lastCity = previousPaths[pathIndex][lengthTrail - 1];
            if (lastCity > NUM_LOCATIONS) {
                printf("pathIndex is %d, numPrevious is %d, lengthTrail - 1 is %d, lastCity is %d\n", pathIndex, numPrevious, lengthTrail - 1, lastCity);
                printf("go up one: %d\n", previousPaths[pathIndex - 1][lengthTrail - 1]);
                printf("go down one: %d\n", previousPaths[pathIndex + 1][lengthTrail - 1]);
                printf("go left one: %d\n", previousPaths[pathIndex][lengthTrail - 2]);
                printf("go left two: %d\n", previousPaths[pathIndex][lengthTrail - 3]);
            }
            fflush(stdout);
            int newIndex = 0;
            // add all possible land moves
            fflush(stdout);
            fflush(stdout);
            for (newIndex = 0; adjacencyRoad[lastCity][newIndex] != END; newIndex ++) {
                fflush(stdout);
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
            fflush(stdout);
            for (newIndex = 0; adjacencySea[lastCity][newIndex] != END; newIndex ++) {
                fflush(stdout);
                LocationID *newPath = malloc(TRAIL_SIZE * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = adjacencyRoad[lastCity][newIndex];
                if (validDraculaTrail(histories, newPath)) {
                    generatedTrails[*numPaths] = newPath;
                    *numPaths = *numPaths + 1;
                } else {
                    free(newPath);
                }
                fflush(stdout);
            }
            fflush(stdout);
            // special move: teleport
            /*
            if (histories[PLAYER_DRACULA][lengthTrail] == TELEPORT) {
                LocationID *newPath = malloc(TRAIL_SIZE * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = CASTLE_DRACULA;
                generatedTrails[*numPaths] = newPath;
                *numPaths = *numPaths + 1;
            }
            */
            
        }
    }
    fflush(stdout);

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
        } else {
            // check that the city is not in the trail
            int j;
            for (j = i + 1; j < TRAIL_SIZE; j ++) {
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


int intPow(int base, int index) {
    if (index == 0) {
        return 1;
    }
    return base * intPow(base, index - 1);
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
    
    // Store all the current state information
    PlayerID player = getCurrentPlayer(hView);
    LocationID playerLoc = getLocation(hView, player);
    Round curRound = getRound(hView);
    
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
    
    int i;
    // Create a probability array of possible locations Dracula could be in this turn
    int probableNow[NUM_MAP_LOCATIONS];
    // Create a probability array of possible locations Dracula could be in next turn
    int probableNext[NUM_MAP_LOCATIONS];
    // Distance from each hunter
    int distance[NUM_PLAYERS - 1][NUM_MAP_LOCATIONS];
    for (i = 0; i < NUM_PLAYERS - 1; i++) {
        findDistances(hView, playerLoc, i, distance[i]);
    }
    // TODO pass frequency...
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        probableNow[i] = 0;
        probableNext[i] = 0;
    }
    
    /*printf("Player %d at %s, distances:\n", player, names[playerLoc]);
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        printf("%s: %d\n", names[i], distance[player][i]);
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
    LocationID destination = 0;
    int highScore = 0;
    LocationID location;
    // Distance multiplier should be really very minor
    // as it is only meant to break ties
    int distanceMultiplier = 10 * numPaths;
    // printf("%d possible paths\n", numPaths);
    // goDirect is a flag to specifies whether to go directly to a point
    // (usually adjacent square) or to take the path which spreads out the most.
    int goDirect = FALSE;
    for (location = 0; location < NUM_MAP_LOCATIONS; location++) {
        // How far are we from this location being considered?
        // The closer we are the more relevant it is
        LocationID *path = NULL;
        int distanceToLoc = 20 - shortestPath(hView, playerLoc, location, &path);
        free(path);
        int curScore = 0;
        int urgentCheck = FALSE;
        // If we are next to the location being considered, high chance
        // that we want to investigate it if there's a chance he's right there
        if (inArray(adjLocs, location, numAdjLocs)) {
            urgentCheck = TRUE;
            curScore += 500 * probableNow[location] * numPaths;
        }
        curScore += 100 * probableNext[location] * numPaths;
        int k;
        for (k = 0; k < NUM_PLAYERS - 1; k++) {
            if (k != player) {
                curScore += distanceMultiplier * distance[k][location];
            }
        }
        curScore *= distanceToLoc;
        curScore /= 20;
        if (curScore > highScore) {
            if (urgentCheck == TRUE) {
                goDirect = TRUE;
                //printf("Location %s is a good place to URGENTLY check out\n", names[location]);
            } else {
                goDirect = FALSE;
                //printf("Location %s is a good place to check out\n", names[location]);
            }
            highScore = curScore;
            destination = location;
        }
    }
    //printf("I really think the best place to go is %s\n", names[destination]);
    
    LocationID firstStep = playerLoc; // Default move
    if (goDirect) {
        //printf("Go direct! Get 'em boys!\n");
        // Get the first step of the optimal path towards our destination
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
            int i;
            for (i = 0; i < length; i++) {
                printf("i is %d\n", i);
                printf("pathToTake[i] is %d\n", pathToTake[i]);
                printf("%s ", names[pathToTake[i]]);
            }
            printf("\n");*/
        }
        //printf("Thus the first step to take is %s\n", names[firstStep]);
    } else {
        //printf("Spreading out\n");
        // Consider all first steps
        // Score it based on shortest distance and greatest spread
        int k;
        int bestScore = 0;
        for (k = 0; k < numAdjLocs; k++) {
            //printf("iterating through adjLoc %d(%s)\n", k, names[k]);
            //printf("destination is %d(%s)\n", destination, names[destination]);
            LocationID *pathToTake = NULL;
            int distanceScore = shortestPath(hView, adjLocs[k], destination, &pathToTake);
            //printf("found shortest path!\n");
            assert(distanceScore != -1);
            free(pathToTake);
            int spreadScore = 0;
            int h;
            for (h = 0; h < NUM_PLAYERS - 1; h++) {
                if (h != player) {
                    spreadScore += distance[h][adjLocs[k]];
                }
            }
            
            int totalScore = 200 + (5 * spreadScore) - (10 * distanceScore);
            //printf("City: %s, Dist: %d, Spread: %d, Total: %d\n", names[adjLocs[k]], distanceScore, spreadScore, totalScore);
            if (totalScore > bestScore) {
                //printf("Best score, go for it\n");
                bestScore = totalScore;
                firstStep = adjLocs[k];
            }
        }
        //printf("First step to tak is %s\n", names[firstStep]);
    }
            
    free(adjLocs);
    strcpy(bestMove, names[firstStep]);
}
