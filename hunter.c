#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

static int possibleDraculaLocations(HunterView hView, possible[NUM_LOCATIONS]);

void decideMove(HunterView gameState) {

    registerBestPlay("ZU", "I Wanna Stake You Dracula <3" );
}

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
                    
