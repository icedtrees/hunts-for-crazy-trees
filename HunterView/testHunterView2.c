#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"

#define MAX_LOCATION 71

//A sample test program HunterView.h
int main() {
        int i;
	playerMessage messages[] = {};
	
	printf("Test basic empty initialisation\n");
	HunterView hv = newHunterView("", messages);
	assert(getCurrentPlayer(hv) == PLAYER_LORD_GODALMING);
	assert(getRound(hv) == 0);
	assert(getHealth(hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
	assert(getHealth(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
        assert(getScore(hv) == GAME_START_SCORE);
        assert(getLocation(hv,PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);
        
        printf("passed\n");
	disposeHunterView(hv);
	
        playerMessage messages2[] = {"Hello","Rubbish","Stuff","","Mwahahah"};
	printf("Test for Dracula trail and basic functions\n");
	hv = newHunterView("GST.... SAO.... HZU.... MBB.... DC?....", messages2);
	assert(getCurrentPlayer(hv) == PLAYER_LORD_GODALMING);
	assert(getRound(hv) == 1);
	assert(getLocation(hv, PLAYER_LORD_GODALMING) == STRASBOURG);
	assert(getLocation(hv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
	assert(getLocation(hv, PLAYER_VAN_HELSING) == ZURICH);
	assert(getLocation(hv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
        assert(getLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
	assert(getHealth(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
	printf("passed\n");
        disposeHunterView(hv);
        

	printf("Test for encountering Dracula and hunter history\n");
        playerMessage messages3[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!"};
	hv = newHunterView("GST.... SAO.... HCD.... MAO.... DGE.... GGED...", messages3);
	assert(getLocation(hv,PLAYER_DRACULA) == GENEVA);
	assert(getHealth(hv, PLAYER_LORD_GODALMING) == 5);
	assert(getHealth(hv,PLAYER_DRACULA) == 30);
	assert(getLocation(hv, PLAYER_LORD_GODALMING) == GENEVA);
        LocationID history[TRAIL_SIZE];
	getHistory(hv, PLAYER_DRACULA,history);
        assert(history[0] == GENEVA);
        assert(history[1] == UNKNOWN_LOCATION);
        getHistory(hv, PLAYER_LORD_GODALMING,history);
        assert(history[0] == GENEVA);
        assert(history[1] == STRASBOURG);
        assert(history[2] == UNKNOWN_LOCATION);
        getHistory(hv,PLAYER_DR_SEWARD,history);
        assert(history[0] == ATLANTIC_OCEAN);
        assert(history[1] == UNKNOWN_LOCATION);
        printf("passed\n");        
	disposeHunterView(hv);


	printf("Test for dracula doubling back at sea, and losing blood points\n");
	playerMessage messages4[] = {"Hello","Rubbish","Stuff","","Mwahahah","Aha!","","","","Back I go"};
        hv = newHunterView("GGE.... SGE.... HGE.... MGE.... DS?.... GST.... SST.... HST.... MST.... DD1....", messages4);
        
        assert(getLocation(hv,PLAYER_DRACULA) == DOUBLE_BACK_1);
        getHistory(hv,PLAYER_DRACULA,history);
        assert(history[0] == DOUBLE_BACK_1);
	assert(history[1] == SEA_UNKNOWN);
	assert(getHealth(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);
	assert(getCurrentPlayer(hv) == 0);
        printf("passed\n");
	disposeHunterView(hv);

	fflush(stdout);

	printf("Test for connections\n");
	int seen[MAX_LOCATION], *edges;
	hv = newHunterView("", messages);	
	
        int size;
	edges = connectedLocations(hv,&size, GALATZ,PLAYER_LORD_GODALMING,0,1,0,0);
	memset(seen, 0, MAX_LOCATION*sizeof(int));
	for (i = 0; i< size ; i++) {
		seen[edges[i]] = 1;
	}
        assert(seen[GALATZ]);
	assert(seen[CONSTANTA] && seen[BUCHAREST] && seen[KLAUSENBURG] && seen[CASTLE_DRACULA]);
	assert(size == 5);
        free(edges);
	
	//Checking Ionian seas
        edges = connectedLocations(hv, &size,IONIAN_SEA, PLAYER_LORD_GODALMING, 0 , 0, 0, 1);
	memset(seen, 0, MAX_LOCATION*sizeof(int));
	for (i = 0; i < size; i++) {
		seen[edges[i]] = 1;
	}
	assert(seen[IONIAN_SEA] && seen[BLACK_SEA] && seen[ADRIATIC_SEA] && seen[TYRRHENIAN_SEA]);
        assert(seen[ATHENS] && seen[VALONA] && seen[SALONICA]);
        assert(size == 7);
        free(edges);

	edges = connectedLocations(hv, &size,ATHENS,PLAYER_LORD_GODALMING,0,0,1,0);
        assert(size == 1);
        assert(edges[0] == ATHENS);
        free(edges);
        printf("passed\n");
        disposeHunterView(hv);
	return 0;
}

