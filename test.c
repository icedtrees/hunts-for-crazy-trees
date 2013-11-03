#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include "locations.h"
#include "dracula.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

void printDraculaTrails(LocationID **trails, int numTrails) {
    int i;
    for (i = 0; i < numTrails; i ++) {
        int j;
        for (j = 0; j < TRAIL_SIZE; j ++) {
            printf("i is %d, j is %d\n", i, j);
            if (trails[i][j] == UNKNOWN_LOCATION) {
                printf("X");
            } else {
                printf("%s", names[trails[i][j]]);
            }
            if (j != TRAIL_SIZE - 1) {
                printf("<--");
            }
        }
        printf("\n");
    }
}

void registerBestPlay( char *play, playerMessage message ) {}

int main(int argc, char *argv[]) {
    playerMessage messages[] = {};
    char *pastPlays = "GJM.... SHA.... HGO.... MTO.... DDU.V.. GSJ.... SNS.... HFL.... MSR.... DHIT... GZA.... SED.... HVE.... MSN.... DC?T... GSZ.... SMN.... HMU.... MAL.... DD3T... GKL.... SLO.... HST.... MBO.... DS?.... GCD.... SLV.... HBU.... MNA.... DLVT... GCD.... SLVTD.. HBU.... MNA.... DMNT.V. GGA.... SSW.... HAM.... MLE.... DHIT.M. GCN.... SLO.... HNS.... MEC.... DEDT.M. GBC.... SPL.... HNS.... MEC.... DD3T.M. GBC.... SPL.... HNS.... MEC.... DLOT... GBC.... SPL.... HNS.... MEC.... DSWT... GBC.... SPL.... HNS.... MEC.... DLVT.M. GSZ.... SLOT... HEDT... MLO.... DHI.VM. GZA.... SMNT... HED.... MLO.... DS?.... GZA.... SMN.... HED.... MLO.... DC?T... GZA.... SMN.... HED.... MLO.... DC?T... GZA.... SMN.... HED.... MLO.... DD2T.M. GZA.... SMN.... HED.... MLO.... DTPT.M. GSZ.... SLO.... HNS.... MEC.... DHIT.V. GKL.... SEC.... HNS.... MPL.... DKLT... GKLTD.. SNS.... HHA.... MEC.... DBCT.M. GBCTD.. SNS.... HNS.... MNS.... DC?T.M. GBE.... SNS.... HEC.... MNS.... DC?T.M. GBC.... SNS.... HNS.... MEC.... DD3T.M. GBCTD.. SED.... HED.... MNS.... DHIT.M. GBCTD.. SED.... HED.... MHA.... DKL.V.. GBE.... SED.... HED.... MPR.... DCDT... GKLV... SED.... HLO.... MSZ.... DTPT.M.";
    HunterView hView = newHunterView(pastPlays, messages);
    int allHistories[NUM_PLAYERS][TRAIL_SIZE];
    PlayerID currentPlayer;
    for (currentPlayer = 0; currentPlayer < NUM_PLAYERS; currentPlayer ++) {
        getHistory(hView, currentPlayer, allHistories[currentPlayer]);
    }
    disposeHunterView(hView);
    
    int numPaths;
    int previousNumPaths;
    int **draculaTrails = getDraculaTrails(allHistories, NULL, &numPaths, 0);
    int **previousTrails = NULL;
    
    printf("\ndepth 0:\n");
    printDraculaTrails(draculaTrails, numPaths);
    
    int depth;
    for (depth = 1; depth < TRAIL_SIZE; depth ++) {
        previousTrails = draculaTrails;
        previousNumPaths = numPaths;
        // Use previous dracula trails to incrementally generate more
        draculaTrails = getDraculaTrails(allHistories, previousTrails, &numPaths, depth);
        // Use all possible dracula trails to evaluate a best move

        printf("\ndepth %d:\n", depth);
        printDraculaTrails(draculaTrails, numPaths);
        
        // free previous trails
        int i;
        for (i = 0; i < previousNumPaths; i ++) {
            free(previousTrails[i]);
        }
        free(previousTrails);
    }
    int i;
    for (i = 0; i < numPaths; i ++) {
        free(draculaTrails[i]);
    }
    free(draculaTrails); // trailing trail
    // haha get it, TRAILing trail
    
    return EXIT_SUCCESS;
}


