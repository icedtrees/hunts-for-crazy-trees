#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"
#include "hunter.h"

void testDraculaKnown(void);

int main(int argc, char *argv[]) {
    testDraculaKnown();
    return EXIT_SUCCESS;
}

void registerBestPlay(char *play, playerMessage message) {
    printf("registered play as %c%c\n", play[0], play[1]);
}

void testDraculaKnown(void) {
    printf("Testing best move for each hunter if we know where drac is...\n");
    playerMessage messages[] = {};
    HunterView game = newHunterView("GBE.... SBR.... HLO.... MCA.... DAL.V..", messages);
    decideMove(game);
    disposeHunterView(game);
    
    game = newHunterView("GBE.... SBR.... HLO.... MCA.... DAL.V.. GSJ....", messages);
    decideMove(game);
    disposeHunterView(game);
    
    game = newHunterView("GBE.... SBR.... HLO.... MCA.... DAL.V.. GSJ.... SHA....", messages);
    decideMove(game);
    disposeHunterView(game);
    
    game = newHunterView("GBE.... SBR.... HLO.... MCA.... DAL.V.. GSJ.... SHA.... HEC....", messages);
    decideMove(game);
    disposeHunterView(game);
    
    printf("No segfaults have occurred!\n");
}
