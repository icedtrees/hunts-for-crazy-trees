// HunterView.c
// Davy Mao and Leo Huang, October 2013
// A HunterView implementation that stores data for the hunters

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "game.h"
#include "HunterView.h"
#include "locations.h"


// HunterView implementation
typedef struct _node *Node;
typedef struct _node {
    int value;
    Node next;
    Node prev;
} node;

typedef struct _moveList *MoveList;
typedef struct _moveList {
    int movesMade;
    Node first;
    Node last;
} moveList;

typedef struct _player *Player;
typedef struct _player {
    int health;
    LocationID curLoc;
    MoveList moves;
} player;
     
struct hunterView {
    Round curRound;
    PlayerID curPlayer;
    int curScore;
    Player players[NUM_PLAYERS];
};

static Node newNode(LocationID move) {
    Node n = malloc(sizeof(node));
    n->value = move;
    n->next = NULL;
    n->prev = NULL;

    return n;
}

static MoveList newMoveList() {
    MoveList l = malloc(sizeof(moveList));
    l->movesMade = 0;
    l->first = NULL;
    l->last = NULL;

    return l;
}

static void addMove(MoveList l, LocationID move) {
    assert(l != NULL);
    
    l->movesMade++;
    Node newMove = newNode(move);
    if (l->movesMade == 1) {
        // if this is the first move
        l->first = newMove;
        l->last = newMove;
    } else {
        Node lastNode;
        for (lastNode = l->first; lastNode->next != NULL; lastNode = lastNode->next);
        
        lastNode->next = newMove;
        newMove->prev = lastNode;
        l->last = newMove;
    }
}

static Player newPlayer(int health) {
    Player p = malloc(sizeof(player));
    p->health = health;
    p->curLoc = UNKNOWN_LOCATION;
    p->moves = newMoveList();
    
    return p;
}

static LocationID moveID(char a, char b) {
    char move[3];
    move[0] = a;
    move[1] = b;
    move[2] = '\0';
    
    // credit to Claudia Tu and Daniel Li for this array
    char locations[NUM_LOCATIONS][3] = {
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
        // misc
        "C?", "S?", "HI", "D1", "D2", "D3", "D4", "D5", "TP"
    };
    
    int i;
    for (i = 0; i < NUM_LOCATIONS; i++) {
        if (strcmp(move, locations[i]) == 0) {
            // strings are equal, ID found
            return i;
        }
    }
    
    // move not found in array
    fprintf(stderr, "LocationID not found for move %s\n", move);
    exit(1);
}

static int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

static void simulateGame(HunterView hunterView, char *pastPlays) {
    int i;
    for (i = 0; i < strlen(pastPlays); i++) {
        // read in current play (7 chars)
        char curPlay[7];
        int j;
        for (j = 0; j < 7; j++) {
            curPlay[j] = pastPlays[i];
            i++;
        }
        
        // determine player
        PlayerID curPlayer;
        if (curPlay[0] == 'G') {
            curPlayer = PLAYER_LORD_GODALMING;
        } else if (curPlay[0] == 'S') {
            curPlayer = PLAYER_DR_SEWARD;
        } else if (curPlay[0] == 'H') {
            curPlayer = PLAYER_VAN_HELSING;
        } else if (curPlay[0] == 'M') {
            curPlayer = PLAYER_MINA_HARKER;
        } else if (curPlay[0] == 'D') {
            curPlayer = PLAYER_DRACULA;
        } else {
            fprintf(stderr, "Could not determine player for this play\n");
            exit(1);
        }
        
        // prepare move to be added to player's MoveList
        LocationID curLoc = getLocation(hunterView, curPlayer);
        LocationID dest = moveID(curPlay[1], curPlay[2]);
        LocationID realDest = dest; //real destination, after evaluating hide, doubleback etc
        
        // these only apply to Dracula
        if (dest == HIDE) {
            realDest = curLoc;
        } else if (dest >= DOUBLE_BACK_1 && dest <= DOUBLE_BACK_5) {
            int movesBack = dest - DOUBLE_BACK_1;
            LocationID history[TRAIL_SIZE];
            getHistory(hunterView, curPlayer, history);
            
            realDest = history[movesBack];
            if (dest == HIDE) {
                realDest = history[movesBack + 1];
            }
        }

        // add the move and update curLoc
        addMove(hunterView->players[curPlayer]->moves, dest);
        hunterView->players[curPlayer]->curLoc = dest;
        
        // process each action character
        if (curPlayer == PLAYER_DRACULA) {
            if (curPlay[5] == 'V') {
                // vampire matured, -13 score
                hunterView->curScore -= 13;
            }
            
            // special cases at end of turn
            if (realDest == SEA_UNKNOWN ||
                (realDest >= NORTH_SEA && realDest <= BLACK_SEA)) {
                hunterView->players[curPlayer]->health -= 2;
            } else if (realDest == CASTLE_DRACULA || realDest == TELEPORT) {
                hunterView->players[curPlayer]->health += 10;
            }
        } else {
            // hunter
            for (j = 3; j < 7; j++) {
                if (curPlay[j] == 'T') {
                    // trap activated, -2 health
                    hunterView->players[curPlayer]->health -= 2;
                } else if (curPlay[j] == 'D') {
                    // found Dracula, -4 health and -10 for Dracula
                    hunterView->players[curPlayer]->health -= 4;
                    hunterView->players[PLAYER_DRACULA]->health -= 10;
                }
            }
            if (hunterView->players[curPlayer]->health <= 0) {
                // hunter died, full health and tp to hospital
                hunterView->curScore -= 6;
                hunterView->players[curPlayer]->health = GAME_START_HUNTER_LIFE_POINTS;
                hunterView->players[curPlayer]->curLoc = ST_JOSEPH_AND_ST_MARYS;
            } else {
                if (curLoc == dest) {
                // didn't move, rested
                // TODO find out if rest first or not
                int curHealth = hunterView->players[curPlayer]->health;
                hunterView->players[curPlayer]->health = min(curHealth + 3, GAME_START_HUNTER_LIFE_POINTS);
            }
            }
        }

        // increment player and round
        hunterView->curPlayer = (hunterView->curPlayer + 1) % NUM_PLAYERS;
        if (curPlayer == PLAYER_DRACULA) {
            hunterView->curRound++;
            hunterView->curScore--;
        }
    }
}
        

HunterView newHunterView(char *pastPlays, playerMessage messages[]) {
    HunterView hunterView = malloc(sizeof(struct hunterView));
    hunterView->curRound = 0;
    hunterView->curPlayer = PLAYER_LORD_GODALMING; //dunno who starts
    hunterView->curScore = GAME_START_SCORE;
    
    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i == PLAYER_DRACULA) {
            hunterView->players[i] = newPlayer(GAME_START_BLOOD_POINTS);
        } else {
            hunterView->players[i] = newPlayer(GAME_START_HUNTER_LIFE_POINTS);
        }
    }
    
    simulateGame(hunterView, pastPlays);
    
    return hunterView;
}

// this function frees all memory previously allocated for the HunterView
// toBeDeleted. toBeDeleted should not be accessed after the call.
void disposeHunterView(HunterView toBeDeleted) {
    // i will do this now
    int i;
    Node previousMove = NULL;
    Node currentMove = NULL;
    for (i = 0; i < NUM_PLAYERS; i ++) {
        currentMove = toBeDeleted->players[i]->moves->first;
        while (currentMove != NULL) {
            previousMove = currentMove;
            currentMove = currentMove->next;
            free(previousMove);
        }
        free(toBeDeleted->players[i]->moves);
        free(toBeDeleted->players[i]);
    }
    free(toBeDeleted);
}

Round getRound (HunterView currentView) {
    return currentView->curRound;
}

PlayerID getCurrentPlayer (HunterView currentView) {
    return currentView->curPlayer;
}

int getScore(HunterView currentView) {
    return currentView->curScore;
}

int getHealth(HunterView currentView, PlayerID player) {
    return currentView->players[player]->health;
}

LocationID getLocation(HunterView currentView, PlayerID player) {
    return currentView->players[player]->curLoc;
}

void getHistory(HunterView currentView, PlayerID player, LocationID trail[TRAIL_SIZE]) {
    int i;
    // set all to UNKNOWN_LOCATION (-1) first
    for (i = 0; i < TRAIL_SIZE; i++) {
        trail[i] = UNKNOWN_LOCATION;
    }
    
    // starting from the most recent move
    MoveList moves = currentView->players[player]->moves;
    Node curMove = moves->last;
    
    i = 0;
    while (curMove != NULL && i < TRAIL_SIZE) {
        trail[i] = curMove->value;
        i++;
        curMove = curMove->prev;
    }
}

LocationID * connectedLocations(HunterView currentView, int *numLocations, LocationID from, 
                              PlayerID player, Round round, int road, int rail, int sea) {
    LocationID *result = malloc(NUM_MAP_LOCATIONS * sizeof(LocationID));
    int adjacentLocations = 0;
    int i;
    // include current city
    result[adjacentLocations] = from;
    adjacentLocations ++;
    if (road) {
        for (i = 0; adjacencyRoad[from][i] != END; i++) {
            if (player == PLAYER_DRACULA && result[adjacentLocations] == ST_JOSEPH_AND_ST_MARYS) {
                continue; // dracula cannot go there
            }
            result[adjacentLocations] = adjacencyRoad[from][i];
            adjacentLocations++;
        }
    }
    if (sea) {
        for (i = 0; adjacencySea[from][i] != END; i++) {
            result[adjacentLocations] = adjacencySea[from][i];
            adjacentLocations++;
        }
    }
    if (rail && player != PLAYER_DRACULA) {
        if (round % 4 == 1) {
            for (i = 0; adjacencyRail1[from][i] != END; i++) {
                result[adjacentLocations] = adjacencyRail1[from][i];
                adjacentLocations++;
            }
        } else if (round % 4 == 2) {
            for (i = 0; adjacencyRail2[from][i] != END; i++) {
                result[adjacentLocations] = adjacencyRail2[from][i];
                adjacentLocations++;
            }        
        } else if (round % 4 == 3) {
            for (i = 0; adjacencyRail3[from][i] != END; i++) {
                result[adjacentLocations] = adjacencyRail3[from][i];
                adjacentLocations++;
            }
        }
    }
    *numLocations = adjacentLocations;
    return result;
}


