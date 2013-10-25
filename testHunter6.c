#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"
#include "locations.h"
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
    HunterView hView = newHunterView(
    "GJM.... SJM.... HJM.... MJM.... DLS.V.. "
    "GBE.... SBE.... HBE.... MBE.... DC?T... "
    "GBE.... SBE.... HBE.... MBE.... DC?T... "
    "GSA....", messages);
    
    printf("connected locations from BE:\n");
    int numConnected;
    LocationID *connected = connectedLocations(hView, &numConnected, getLocation(hView, getCurrentPlayer(hView)), getCurrentPlayer(hView), getRound(hView), TRUE, TRUE, TRUE);
    int i;
    for (i = 0; i < numConnected; i ++) {
        printf("BE is connected to %d(%s)\n", connected[i], names[connected[i]]);
    }
    free(connected);
    decideMove(hView);
    disposeHunterView(hView);
    
    
    printf("No segfaults have occurred!\n");
}
