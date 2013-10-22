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
   LocationID *path = NULL;
   int length = shortestPath(hView, BELGRADE, VENICE, &path);
   
   printf("Shortest path: %d\n", length);
   int i;
   for (i = 0; i < length; i++) {
      printf("%s ", names[path[i]]);
   }
   printf("\n");
   free(path);
   
   return EXIT_SUCCESS;
}
