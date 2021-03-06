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
    printf("Testing best move for each hunter if we don't know where drac is...\n");
    playerMessage messages[] = {};
    HunterView game = newHunterView("GBE.... SBR.... HLO.... MCA.... DLO.V.. "
                         "GSJ.... SHA.... HEC.... MAO.... DC?.T.. "
                         "GSJ.... SHA.... HEC.... MAO.... DC?.T.. "
                         "GSJ.... SHA.... HEC.... MAO.... DC?.T.. "
                         "GSJ.... SHA.... HEC.... MAO.... DC?.T.. "
                         "GSJ.... SHA.... HEC.... MAO.... DC?.T..", messages);
    decideMove(game);
    disposeHunterView(game);
    
    printf("No segfaults have occurred!\n");
}
