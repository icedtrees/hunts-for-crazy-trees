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
    playerMessage messages[] = {};
    HunterView game = newHunterView("GBE.... SBR.... HLO.... MCA.... DC?.V..", messages);
    decideMove(game);
    disposeHunterView(game);
    
    game = newHunterView("GBE.... SBR.... HLO.... MCA.... DC?.V.. GSJ....", messages);
    decideMove(game);
    disposeHunterView(game);
    
    game = newHunterView("GBE.... SBR.... HLO.... MCA.... DC?.V.. GSJ.... SHA....", messages);
    decideMove(game);
    disposeHunterView(game);
    
    game = newHunterView("GBE.... SBR.... HLO.... MCA.... DC?.V.. GSJ.... SHA.... HEC....", messages);
    decideMove(game);
    disposeHunterView(game);
    
    printf("No segfaults have occurred!\n");
}
