#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"
#include "hunter.h"

static const char names[NUM_LOCATIONS][3] = {
    // 61 cities
    "AL", "AM", "AT", "BA", "BI", "BE", "BR", "BO", "BU", "BC",
    "BD", "CA", "CG", "CD", "CF", "CO", "CN", "DU", "ED", "FL",
    "FR", "GA", "GW", "GE", "GO", "GR", "HA", "JM", "KL", "LE",
    "LI", "LS", "LV", "LO", "MA", "MN", "MR", "MI", "MU", "NA",
    "NP", "NU", "PA", "PL", "PR", "RO", "SA", "SN", "SR", "SJ",
    "SO", "ST", "SW", "SZ", "TO", "VA", "VR", "VE", "VI", "ZA",
    "ZU",
    // 10 seas
    "NS", "EC", "IS", "AO", "BB", "MS", "TS", "IO", "AS", "BS",
};

void registerBestPlay(char *play, playerMessage message) {
    printf("registered play as %c%c\n", play[0], play[1]);
}

int main (int argc, char **argv) {
   playerMessage messages[] = {};
   HunterView hView = newHunterView("", messages);
   
   LocationID **draculaPaths;
   draculaPaths = malloc(2 * 4);
   draculaPaths[0] = malloc(6 * 4);
   draculaPaths[1] = malloc(6 * 4);
   draculaPaths[0][0] = ALICANTE;
   draculaPaths[0][1] = MADRID;
   draculaPaths[0][2] = LISBON;
   draculaPaths[0][3] = CADIZ;
   draculaPaths[0][4] = ATLANTIC_OCEAN;
   draculaPaths[0][5] = GALWAY;
   draculaPaths[1][0] = BARCELONA;
   draculaPaths[1][1] = TOULOUSE;
   draculaPaths[1][2] = BORDEAUX;
   draculaPaths[1][3] = NANTES;
   draculaPaths[1][4] = PARIS;
   draculaPaths[1][5] = GENEVA;
   printf("Sample draculaPaths created\n");
   
   char bestMove[3];
   getBestMove(hView, bestMove, draculaPaths, 2);
   
   printf("%s\n", bestMove);
   free(draculaPaths[0]);
   free(draculaPaths[1]);
   
   return EXIT_SUCCESS;
}
