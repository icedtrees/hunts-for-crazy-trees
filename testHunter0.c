#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"
#include "hunter.h"

void testNewGame(void);

int main(int argc, char *argv[]) {
    testNewGame();
    return EXIT_SUCCESS;
}

void registerBestPlay(char *play, playerMessage message) {
    printf("registered play as %c%c\n", play[0], play[1]);
}

void testNewGame(void) {
    printf("Testing best move for each hunter on round 0...\n");
    char *pastPlays = "";
    playerMessage messages[] = {}; // new game
    HunterView game = newHunterView(pastPlays, messages);
    decideMove(game);
    disposeHunterView(game);
    printf("No segfaults have occurred!\n");
}
