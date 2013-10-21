// By Davy Mao and Leo Huang
// 13 October 2013
// A test program to make sure the HunterView functions for Phase 1 are working correctly.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "HunterView.h"

void testNewGame(void);
void testConnectedLocations(void);

int main(int argc, char *argv[]) {
    testNewGame();
    testConnectedLocations();
}

void testNewGame(void) {
    printf("Testing making a new game...\n");
    char *pastPlays = "";
    char *messages = ""; // new game
    HunterView game = newHunterView(pastPlays, messages);
    
    assert(getRound(game) == 0);
    assert(getCurrentPlayer(game) == PLAYER_LORD_GODALMING);
    
    assert(getHealth(game, PLAYER_LORD_GODALMING == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_DR_SEWARD == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS));
    
    assert(getScore(game) == GAME_START_SCORE);
    
    assert(getLocation(game, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
    
    locationID resultTrail[TRAIL_SIZE];
    getHistory(game, PLAYER_LORD_GODALMING, resultTrail);
    locationID expectedTrail[TRAIL_SIZE];
    memset(expectedTrail, -1, TRAIL_SIZE);
    assert(memcmp(resultTrail, expectedTrail, TRAIL_SIZE * sizeof(int)) == 0);
    
    disposeHunterView(game);
    printf("Making a new game passed!\n\n");
}

void testConnectedLocations(void) {
    printf("Testing connected locations in starting position...\n");
    char *pastPlays = "";
    char *messages = "";
    HunterView game = newHunterView(pastPlays, messages);
    // It is the hunter's turn
    
    // Test VALONA by road
    int numAdjacent;
    int *connected = connectedLocations(game, &numAdjacent, VALONA, PLAYER_LORD_GODALMING, 0, 1, 0, 0);
    assert(numAdjacent == 4);
    
    // Test VALONA by rail (no rail move permitted, since turn 0)
    connected = connectedLocations(game, &numAdjacent, VALONA, PLAYER_LORD_GODALMING, 0, 0, 1, 0);
    assert(numAdjacent == 0);
    
    // Test SANTANDER by sea
    connected = connectedLocations(game, &numAdjacent, SANTANDER, PLAYER_LORD_GODALMING, 0, 0, 0, 1);
    assert(numAdjacent == 2);
    
    // Test ALICANTE by sea
    connected = connectedLocations(game, &numAdjacent, ALICANTE, PLAYER_LORD_GODALMING, 0, 0, 0, 1);
    assert(numAdjacent == 3);
    
    disposeHunterView(game);
    printf("Testing connected locations passed!\n");
}

void testGetRound(void) {
    char *pastPlays = "";
    char *messages = "";
    HunterView game = 
}