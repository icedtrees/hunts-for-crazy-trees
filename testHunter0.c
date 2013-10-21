#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    testNewGame();
    return EXIT_SUCCESS;
}

void registerBestPlay(char *play, playerMessage message) {
    printf("registered play as %c%c\n", play[0], play[1]);
}

void testNewGame(void) {
    char *pastPlays = "";
    playerMessage messages[] = {}; // new game
    HunterView game = newHunterView(pastPlays, messages);
    
    assert(getRound(game) == 0);
    assert(getCurrentPlayer(game) == PLAYER_LORD_GODALMING);
    
    assert(getHealth(game, PLAYER_LORD_GODALMING == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_DR_SEWARD == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_VAN_HELSING == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_MINA_HARKER == GAME_START_HUNTER_LIFE_POINTS));
    assert(getHealth(game, PLAYER_DRACULA == GAME_START_BLOOD_POINTS));
    
    assert(getScore(game) == GAME_START_SCORE);
    
    assert(getLocation(game, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
    
    LocationID resultTrail[TRAIL_SIZE];
    getHistory(game, PLAYER_LORD_GODALMING, resultTrail);
    LocationID expectedTrail[TRAIL_SIZE];
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        expectedTrail[i] = -1;
    }
    assert(memcmp(resultTrail, expectedTrail, TRAIL_SIZE * sizeof(int)) == 0);
    
    printf("Testing best move for each hunter on round 0...\n");
    PlayerID currentPlayer;
    for (currentPlayer = 0; currentPlayer < NUM_PLAYERS; currentPlayer ++) {
        decideMove(hView);
    }
    printf("No segfaults have occurred!\n");
    
    disposeHunterView(game);
}
