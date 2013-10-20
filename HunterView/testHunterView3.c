#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"

void testHunterView(void);


int main (void) {

  testHunterView();

  return 0;
}

void testHunterView(void) {
    {
        printf("Test 1 Testing just after hunter teleported to hospital...\n");
        playerMessage messages[11] = {"aoeu", "snth", "ABC123", "a", "", "", "a", "b" "c", "d"};
     
        char pastPlays[] = "GMA.... SNP.... HNP.... MNP.... DSR.V.. "
                           "GSRVD.. SNP.... HNP.... MNP.... DTOT... "
                           "GTOTD..";
        HunterView hv = newHunterView(pastPlays, messages);

        
        assert(getHealth(hv,PLAYER_LORD_GODALMING)== GAME_START_HUNTER_LIFE_POINTS);
        assert(getLocation(hv,PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS);
        
        LocationID history[TRAIL_SIZE];
        getHistory(hv,PLAYER_LORD_GODALMING,history);
        assert(history[0] == TOULOUSE);
        assert(history[1] == SARAGOSSA);
        assert(history[2] == MADRID);
        assert(history[3] == UNKNOWN_LOCATION);
        
        assert(getHealth(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2*LIFE_LOSS_HUNTER_ENCOUNTER);
        
        
        disposeHunterView(hv);
        printf("Test 1 passed\n");

    }
    
    {
        printf("Test 2 Testing before next turn for hunter teleported to hospital...\n");
        playerMessage messages[16] = {"aoeu", "snth", "ABC123", "a", "", "", "a", "b" "c", "d"};
        char pastPlays[] =  "GMA.... SNP.... HNP.... MNP.... DSR.V.. "
                            "GSRVD.. SNP.... HNP.... MNP.... DTOT... "
                            "GTOTD.. SNP.... HNP.... MNP.... DC?T...";   
        HunterView hv = newHunterView(pastPlays, messages);
 
        assert(getHealth(hv,PLAYER_LORD_GODALMING)== GAME_START_HUNTER_LIFE_POINTS);
        assert(getLocation(hv,PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS);
        
        LocationID history[TRAIL_SIZE];
        getHistory(hv,PLAYER_LORD_GODALMING,history);
        assert(history[0] == TOULOUSE);
        assert(history[1] == SARAGOSSA);
        assert(history[2] == MADRID);
        assert(history[3] == UNKNOWN_LOCATION);
        
        assert(getHealth(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2*LIFE_LOSS_HUNTER_ENCOUNTER);

        disposeHunterView(hv);

    }
   
    {
        printf("Test 3 Testing after next turn for the teleported hunter\n");
        playerMessage messages[17] = {"aoeu", "snth", "ABC123", "a", "", "", "a", "b" "c", "d"};

        char pastPlays[] = "GMA.... SNP.... HNP.... MNP.... DSR.V.. "
                           "GSRVD.. SNP.... HNP.... MNP.... DTOT... "
                           "GTOTD.. SNP.... HNP.... MNP.... DC?T... "
                           "GSJ....";
  
        HunterView hv = newHunterView(pastPlays, messages);

        assert(getHealth(hv,PLAYER_LORD_GODALMING)== GAME_START_HUNTER_LIFE_POINTS);
        assert(getLocation(hv,PLAYER_LORD_GODALMING) == SARAJEVO);
        
        LocationID history[TRAIL_SIZE];
        getHistory(hv,PLAYER_LORD_GODALMING,history);
        assert(history[0] == SARAJEVO);
        assert(history[1] == TOULOUSE);
        assert(history[2] == SARAGOSSA);
        assert(history[3] == MADRID);
        assert(history[4] == UNKNOWN_LOCATION);
        
        assert(getHealth(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 2*LIFE_LOSS_HUNTER_ENCOUNTER);
        
        disposeHunterView(hv);
        printf("Test 3 passed\n");
    }
    
    {
        printf("Test 4 Testing moving back to place hunter was teleported from\n");
        playerMessage messages[17] = {"aoeu", "snth", "ABC123", "a", "", "", "a", "b" "c", "d"};

        char pastPlays[] = "GSZ.... SNP.... HNP.... MNP.... DBE.V.. "
                           "GBEVD.. SNP.... HNP.... MNP.... DSJT... "
                           "GSJTD.. SNP.... HNP.... MNP.... DSJT... "
                           "GSJTD..";
        HunterView hv = newHunterView(pastPlays, messages);
        int hunterHealth = GAME_START_HUNTER_LIFE_POINTS - 
                           LIFE_LOSS_TRAP_ENCOUNTER - 
                           LIFE_LOSS_DRACULA_ENCOUNTER;
        assert(getHealth(hv,PLAYER_LORD_GODALMING) == hunterHealth);
        assert(getLocation(hv,PLAYER_LORD_GODALMING) == SARAJEVO);
        
        LocationID history[TRAIL_SIZE];
        getHistory(hv,PLAYER_LORD_GODALMING,history);
        assert(history[0] == SARAJEVO);
        assert(history[1] == SARAJEVO);
        assert(history[2] == BELGRADE);
        assert(history[3] == SZEGED);
        assert(history[4] == UNKNOWN_LOCATION);
        
        assert(getHealth(hv,PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 3*LIFE_LOSS_HUNTER_ENCOUNTER);
        
        disposeHunterView(hv);
        printf("Test 4 passed\n");
    
    }

    
    printf("\nAll tests passed\n");
}
