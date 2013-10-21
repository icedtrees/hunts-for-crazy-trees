#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "cities.h"
#include "game.h"
#include "HunterView.h"

int main() {
    LocationID history[TRAIL_SIZE];
    HunterView hv;
    {
	printf("Test for Dracula moving to CD to heal\n");
       
        playerMessage messages[10] = {"aoeu"};
	hv = newHunterView("GBE.... SBU.... HCO.... MCN.... DC?.... "
                           "GLI.... SBU.... HCO.... MCN.... DCD....", messages);
        assert(getLocation(hv,PLAYER_DRACULA) == CASTLE_DRACULA);
        
        int bloodPoints = GAME_START_BLOOD_POINTS + LIFE_GAIN_CASTLE_DRACULA;
        assert(getHealth(hv,PLAYER_DRACULA) == bloodPoints);
        getHistory(hv,PLAYER_DRACULA,history);
        assert(history[0] == CASTLE_DRACULA);
        
	disposeHunterView(hv);
        printf("passed\n");
    }

    {
	printf("Test for Dracula teleporting\n");
        playerMessage messages[22] = {"aoeu"};
	hv = newHunterView("GJM.... SCD.... HAT.... MGO.... DC?.... "
                           "GJM.... SCD.... HVA.... MGO.... DC?.... "
                           "GJM.... SCD.... HAT.... MGO.... DD2.... "
                           "GJM.... SCD.... HVA.... MGO.... DC?.... "
                           "GJM.... SCD.... HAT.... MGO.... DTP.... "
                           "GJM.... SCDD...", messages);
        getHistory(hv,PLAYER_DRACULA,history);
        assert(getLocation(hv,PLAYER_DRACULA) == TELEPORT);

        assert(history[5] == UNKNOWN_LOCATION);
        assert(history[4] == CITY_UNKNOWN);
        assert(history[3] == CITY_UNKNOWN );
        assert(history[2] == DOUBLE_BACK_2);
        assert(history[1] == CITY_UNKNOWN);
        assert(history[0] == TELEPORT);

	assert(getHealth(hv,PLAYER_DRACULA) == 40);
	disposeHunterView(hv);
        printf("passed\n");
    }
    printf("Passed all tests. Your Hunter View is AWESOME!!!!!!!\n");
    return 0;
}

