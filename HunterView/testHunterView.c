#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"

void testNewGame(void);
void testConnectedLocations(void);
void verifyConnectedLocations(void);

int main(int argc, char * argv[]) {
    playerMessage messages[] = {};

    printf("Test basic empty initialisation...");
    HunterView v = newHunterView("", messages);
    assert(getCurrentPlayer(v) == PLAYER_LORD_GODALMING);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    assert(getRound(v) == 0);
    assert(getScore(v) == GAME_START_SCORE);
    printf("Passed!\n");

    printf("Test basic case...");
    v = newHunterView("GBE.... SBR.... HLO.... MCA.... DC?.V..", messages);
    assert(getCurrentPlayer(v) == PLAYER_LORD_GODALMING);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == BELGRADE);
    assert(getLocation(v, PLAYER_DR_SEWARD) == BERLIN);
    assert(getLocation(v, PLAYER_VAN_HELSING) == LONDON);
    assert(getLocation(v, PLAYER_MINA_HARKER) == CADIZ);
    assert(getLocation(v, PLAYER_DRACULA) == CITY_UNKNOWN);
    assert(getHealth(v, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    assert(getRound(v) == 1);
    assert(getScore(v) == GAME_START_SCORE - 1);
    printf("Passed!\n");

    printf("Test case with meeting dracula...");
    v = newHunterView("GBE.... SBR.... HLO.... MCA.... DSJ.V.. GSJVD..", messages);
    assert(getCurrentPlayer(v) == PLAYER_DR_SEWARD);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == SARAJEVO);
    assert(getLocation(v, PLAYER_DR_SEWARD) == BERLIN);
    assert(getLocation(v, PLAYER_VAN_HELSING) == LONDON);
    assert(getLocation(v, PLAYER_MINA_HARKER) == CADIZ);
    assert(getLocation(v, PLAYER_DRACULA) == SARAJEVO);
    assert(getHealth(v, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - 4);
    assert(getHealth(v, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 10);
    assert(getRound(v) == 1);
    assert(getScore(v) == GAME_START_SCORE - 1);
    printf("Passed!\n");

    printf("Test case focusing on health and score...");
    v = newHunterView("GBE.... SBR.... HLO.... MCA.... DSJ.V.. "
                      "GSJVD.. SHATT.. HCG.... MCA.... DHA..V. "
                      "GSJ.... SBR.... HCG.... MCA....", messages);
    assert(getCurrentPlayer(v) == PLAYER_DRACULA);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == SARAJEVO);
    assert(getLocation(v, PLAYER_DR_SEWARD) == BERLIN);
    assert(getLocation(v, PLAYER_VAN_HELSING) == CAGLIARI);
    assert(getLocation(v, PLAYER_MINA_HARKER) == CADIZ);
    assert(getLocation(v, PLAYER_DRACULA) == HAMBURG);
    assert(getHealth(v, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS - 1);
    assert(getHealth(v, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS - 4);
    assert(getHealth(v, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 10);
    assert(getRound(v) == 2);
    assert(getScore(v) == GAME_START_SCORE - 2 - 13);
    printf("Passed!\n");

    printf("Test case focusing on dracula's hide/double-back...");
    v = newHunterView("GBE.... SBR.... HLO.... MCA.... DSJ.V.. "
                      "GSJVD.. SHATT.. HCG.... MCA.... DHA..V. "
                      "GSJ.... SBR.... HCG.... MCA.... DS?.... "
                      "GSJ.... SBRT... HCG.... MCA.... DD1.VV. "
                      "GSJ....", messages);
    assert(getCurrentPlayer(v) == PLAYER_DR_SEWARD);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == SARAJEVO);
    assert(getLocation(v, PLAYER_DR_SEWARD) == BERLIN);
    assert(getLocation(v, PLAYER_VAN_HELSING) == CAGLIARI);
    assert(getLocation(v, PLAYER_MINA_HARKER) == CADIZ);
    assert(getLocation(v, PLAYER_DRACULA) == DOUBLE_BACK_1);
    assert(getHealth(v, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS - 3);
    assert(getHealth(v, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 10 - 4);
    assert(getRound(v) == 4);
    assert(getScore(v) == GAME_START_SCORE - 4 - 26);
    LocationID historyG[] = {SARAJEVO, SARAJEVO, SARAJEVO, SARAJEVO, BELGRADE, -1};
    LocationID historyS[] = {BERLIN, BERLIN, HAMBURG, BERLIN, -1, -1};
    LocationID historyH[] = {CAGLIARI, CAGLIARI, CAGLIARI, LONDON, -1, -1};
    LocationID historyM[] = {CADIZ, CADIZ, CADIZ, CADIZ, -1, -1};
    LocationID historyD[] = {DOUBLE_BACK_1, SEA_UNKNOWN, HAMBURG, SARAJEVO, -1, -1};
    LocationID trail[TRAIL_SIZE];
    getHistory(v, PLAYER_LORD_GODALMING, trail);
    assert(memcmp(trail, historyG, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_DR_SEWARD, trail);
    assert(memcmp(trail, historyS, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_VAN_HELSING, trail);
    assert(memcmp(trail, historyH, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_MINA_HARKER, trail);
    assert(memcmp(trail, historyM, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_DRACULA, trail);
    assert(memcmp(trail, historyD, TRAIL_SIZE*sizeof(LocationID)) == 0);
    printf("Passed!\n");

    printf("Test case focusing on dracula healing and hunter death...");
    v = newHunterView("GBE.... SBR.... HLO.... MCA.... DSJ.V.. "
                      "GSJVD.. SHATT.. HCG.... MCA.... DHA..V. "
                      "GSJ.... SBR.... HCG.... MCA.... DS?.... "
                      "GSJ.... SBRT... HCG.... MCA.... DD1.VV. "
                      "GSJ.... SBRT... HCGDT.. MCA.... DCD.... "
                      "GSJ.... SBRT... HSJDT.. MCA.... DCD.... "
                      "GSJ.... SBRT... HSJDT..", messages);
    assert(getCurrentPlayer(v) == PLAYER_MINA_HARKER);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == SARAJEVO);
    assert(getLocation(v, PLAYER_DR_SEWARD) == BERLIN);
    assert(getLocation(v, PLAYER_VAN_HELSING) == SARAJEVO);
    assert(getLocation(v, PLAYER_MINA_HARKER) == CADIZ);
    assert(getLocation(v, PLAYER_DRACULA) == CASTLE_DRACULA);
    assert(getHealth(v, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS - 6);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 20 - 4);
    assert(getRound(v) == 6);
    assert(getScore(v) == GAME_START_SCORE - 6 - 26);
    printf("Passed!\n");
    
    printf("Test case focusing on dracula teleport and histories...");
    v = newHunterView("GBE.... SBR.... HLO.... MCA.... DSJ.V.. "
                      "GSJVD.. SHATT.. HCG.... MCA.... DHA..V. "
                      "GSJ.... SBR.... HCG.... MCA.... DS?.... "
                      "GSJ.... SBRT... HCG.... MCA.... DD1.VV. "
                      "GSJ.... SBRT... HCGDT.. MCA.... DCD.... "
                      "GSJ.... SBRT... HSJDT.. MCA.... DCD.... "
                      "GSJ.... SBRT... HSJDT.. MCA.... DCA.... "
                      "GSJ.... SBRT... HSJ.... MCA.... DTP....", messages);
    assert(getCurrentPlayer(v) == PLAYER_LORD_GODALMING);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == SARAJEVO);
    assert(getLocation(v, PLAYER_DR_SEWARD) == BERLIN);
    assert(getLocation(v, PLAYER_VAN_HELSING) == SARAJEVO);
    assert(getLocation(v, PLAYER_MINA_HARKER) == CADIZ);
    assert(getLocation(v, PLAYER_DRACULA) == TELEPORT);
    assert(getHealth(v, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS - 3);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 10 - 4);
    assert(getRound(v) == 8);
    assert(getScore(v) == GAME_START_SCORE - 8 - 26);
    LocationID historyG2[] = {SARAJEVO, SARAJEVO, SARAJEVO, SARAJEVO, SARAJEVO, SARAJEVO};
    LocationID historyS2[] = {BERLIN, BERLIN, BERLIN, BERLIN, BERLIN, BERLIN};
    LocationID historyH2[] = {SARAJEVO, SARAJEVO, SARAJEVO, CAGLIARI, CAGLIARI, CAGLIARI};
    LocationID historyM2[] = {CADIZ, CADIZ, CADIZ, CADIZ, CADIZ, CADIZ};
    LocationID historyD2[] = {TELEPORT, CADIZ, CASTLE_DRACULA, CASTLE_DRACULA, DOUBLE_BACK_1, SEA_UNKNOWN};
    getHistory(v, PLAYER_LORD_GODALMING, trail);
    assert(memcmp(trail, historyG2, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_DR_SEWARD, trail);
    assert(memcmp(trail, historyS2, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_VAN_HELSING, trail);
    assert(memcmp(trail, historyH2, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_MINA_HARKER, trail);
    assert(memcmp(trail, historyM2, TRAIL_SIZE*sizeof(LocationID)) == 0);
    getHistory(v, PLAYER_DRACULA, trail);
    assert(memcmp(trail, historyD2, TRAIL_SIZE*sizeof(LocationID)) == 0);
    printf("Passed!\n");
    
    printf("Test case focusing on dracula double back 5...");
    v = newHunterView("GBE.... SBR.... HLO.... MCA.... DSJ.V.. "
                      "GSJVD.. SHATT.. HCG.... MCA.... DHA..V. "
                      "GSJ.... SBR.... HCG.... MCA.... DS?.... "
                      "GSJ.... SBRT... HCG.... MCA.... DS?.VV. "
                      "GSJ.... SBRT... HCGDT.. MCA.... DCD.... "
                      "GSJ.... SBRT... HSJDT.. MCA.... DCD.... "
                      "GSJ.... SBRT... HSJDT.. MCA.... DCA.... "
                      "GSJ.... SBRT... HSJ.... MCA.... DTP.... "
                      "GSJ.... SBR.... HSJ.... MCA.... DD5....", messages);
    assert(getCurrentPlayer(v) == PLAYER_LORD_GODALMING);
    assert(getLocation(v, PLAYER_LORD_GODALMING) == SARAJEVO);
    assert(getLocation(v, PLAYER_DR_SEWARD) == BERLIN);
    assert(getLocation(v, PLAYER_VAN_HELSING) == SARAJEVO);
    assert(getLocation(v, PLAYER_MINA_HARKER) == CADIZ);
    assert(getLocation(v, PLAYER_DRACULA) == DOUBLE_BACK_5);
    assert(getHealth(v, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
    assert(getHealth(v, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 10 - 6);
    assert(getRound(v) == 9);
    assert(getScore(v) == GAME_START_SCORE - 9 - 26);
    printf("Passed!\n");

    printf("All passed! Disposing...\n");
    disposeHunterView(v);
    printf("Successfully disposed!\n");

    //Davy's tests
    printf("Now running more tests including testing connectedLocations:\n");
    testNewGame();
    testConnectedLocations();
    verifyConnectedLocations();
    return 0;
}

void testNewGame(void) {
    printf("Testing making a new game...\n");
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
    
    disposeHunterView(game);
    printf("Making a new game passed!\n\n");
}

void testConnectedLocations(void) {
    printf("Testing connected locations in starting position...\n");
    char *pastPlays = "";
    playerMessage messages[] = {};
    HunterView game = newHunterView(pastPlays, messages);
    // It is the hunter's turn
    
    // Test VALONA by road
    int numAdjacent;
    connectedLocations(game, &numAdjacent, VALONA, PLAYER_LORD_GODALMING, 0, TRUE, FALSE, FALSE);
    printf("adjacent is %d\n", numAdjacent);
    assert(numAdjacent == 5); // four roads and self
    
    // Test VALONA by rail (no rail move permitted, since turn 0)
    connectedLocations(game, &numAdjacent, VALONA, PLAYER_LORD_GODALMING, 0, FALSE, TRUE, FALSE);
    assert(numAdjacent == 1); // self
    
    // Test SANTANDER by sea
    connectedLocations(game, &numAdjacent, SANTANDER, PLAYER_LORD_GODALMING, 0, FALSE, FALSE, TRUE);
    assert(numAdjacent == 2); // ocean and self
    
    // Test CAGLIARI by sea 
    connectedLocations(game, &numAdjacent, CAGLIARI, PLAYER_LORD_GODALMING, 0, FALSE, FALSE, TRUE);
    assert(numAdjacent == 3); // 2 oceans and self
    
    disposeHunterView(game);
    printf("Testing connected locations passed!\n");
}

void verifyConnectedLocations(void) {
    printf("Verifying that connections go both ways...\n");
    char *pastPlays = "";
    playerMessage messages[] = {};
    HunterView hView = newHunterView(pastPlays, messages);
    LocationID from;
    for (from = 0; from < NUM_MAP_LOCATIONS; from++) {
        int numDestinations;
        
        // Testing road connections
        LocationID *roadDestinations = connectedLocations(hView, &numDestinations, from, PLAYER_LORD_GODALMING, 0, TRUE, FALSE, FALSE);
        int toIndex;
        for (toIndex = 0; toIndex < numDestinations; toIndex++) {
            LocationID to = roadDestinations[toIndex];
            int numDestinations2;
            LocationID *roadDestinations2 = connectedLocations(hView, &numDestinations2, to, PLAYER_LORD_GODALMING, 0, TRUE, FALSE, FALSE);
            int locationFound = FALSE;
            int search;
            for (search = 0; search < numDestinations2 && !locationFound; search++) {
                if (roadDestinations2[search] == from) {
                    locationFound = TRUE;
                }
            }
            if (!locationFound) {
                printf("road: from is %d, to is %d\n", from, to);
            }
            assert(locationFound);
            free(roadDestinations2);
        }
        free(roadDestinations);
        
        // Testing rail destinations (1 distance)
        LocationID *railDestinations = connectedLocations(hView, &numDestinations, from, PLAYER_LORD_GODALMING, 1, FALSE, TRUE, FALSE);
        for (toIndex = 0; toIndex < numDestinations; toIndex++) {
            LocationID to = railDestinations[toIndex];
            int numDestinations2;
            LocationID *railDestinations2 = connectedLocations(hView, &numDestinations2, to, PLAYER_LORD_GODALMING, 1, FALSE, TRUE, FALSE);
            int locationFound = FALSE;
            int search;
            for (search = 0; search < numDestinations2 && !locationFound; search++) {
                if (railDestinations2[search] == from) {
                    locationFound = TRUE;
                }
            }
            if (!locationFound) {
                printf("rail: from is %d, to is %d\n", from, to);
            }
            assert(locationFound);
            free(railDestinations2);
        }
        free(railDestinations);
        
        // Testing sea destinations
        LocationID *seaDestinations = connectedLocations(hView, &numDestinations, from, PLAYER_LORD_GODALMING, 0, FALSE, FALSE, TRUE);
        for (toIndex = 0; toIndex < numDestinations; toIndex++) {
            LocationID to = seaDestinations[toIndex];
            int numDestinations2;
            LocationID *seaDestinations2 = connectedLocations(hView, &numDestinations2, to, PLAYER_LORD_GODALMING, 0, FALSE, FALSE, TRUE);
            int locationFound = FALSE;
            int search;
            for (search = 0; search < numDestinations2 && !locationFound; search++) {
                if (seaDestinations2[search] == from) {
                    locationFound = TRUE;
                }
            }
            if (!locationFound) {
                printf("sea: from is %d, to is %d\n", from, to);
            }
            assert(locationFound);
            free(seaDestinations2);
        }
        free(seaDestinations);
    }
    printf("Passed!\n");
}
