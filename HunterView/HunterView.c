#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "game.h"
#include "HunterView.h"

#define END -2
#define NUM_CITIES 61
static const int adjacencyRoad[][NUM_MAP_LOCATIONS] = {
    {SARAGOSSA, MADRID, GRANADA, END}, //ALICANTE
    {COLOGNE, BRUSSELS, END}, //AMSTERDAM
    {VALONA, END}, //ATHENS
    {SARAGOSSA, TOULOUSE, END}, //BARCELONA
    {NAPLES, ROME, END}, //BARI
    {SARAJEVO, SOFIA, ST_JOSEPH_AND_ST_MARYS, SZEGED, KLAUSENBURG, BUCHAREST, END}, //BELGRADE
    {HAMBURG, PRAGUE, LEIPZIG, END}, //BERLIN
    {NANTES, CLERMONT_FERRAND, TOULOUSE, SARAGOSSA, END}, //BORDEAUX
    {AMSTERDAM, COLOGNE, STRASBOURG, PARIS, LE_HAVRE, END}, //BRUSSELS
    {CONSTANTA, GALATZ, SOFIA, BELGRADE, KLAUSENBERG, END}, //BUCHAREST
    {KLAUSENBERG, SZEGED, VIENNA, ZAGREB, END}, //BUDAPEST
    {GRANADA, MADRID, LISBON, END}, //CADIZ
    {END}, //CAGLIARI
    {GALATZ, KLAUSENBERG, END}, //CASTLE_DRACULA
    {PARIS, GENEVA, MARSEILLES, TOULOUSE, BORDEAUX, NANTES, END}, //CLERMONT_FERRAND
    {AMSTERDAM, HAMBURG, LEIPZIG, FRANKFURT, STRASBOURG, BRUSSELS, END}, //COLOGNE
    {GALATZ, BUCHAREST, VARNA, END}, //CONSTANTA
    {GALWAY, END}, //DUBLIN
    {MANCHESTER, END}, //EDINBURGH
    {VENICE, ROME, GENOA, END}, //FLORENCE
    {COLOGNE, LEIPZIG, NUREMBURG, STRASBOURG, END}, //FRANKFURT
    {CASTLE_DRACULA, CONSTANTA, BUCHAREST, KLAUSENBERG, END}, //GALATZ
    {DUBLIN, END}, //GALWAY
    {STRASBOURG, ZURICH, MARSEILLES, CLERMONT_FERRAND, PARIS, END}, //GENEVA
    {MILAN, VENICE, FLORENCE, MARSEILLES, END}, //GENOA
    {MADRID, ALICANTE, CADIZ, END}, //GRANADA
    {BERLIN, LEIPZIG, COLOGNE, END}, //HAMBURG
    {SZEGED, BELGRADE, SARAJEVO, ZAGREB, END}, //ST_JOSEPH_AND_ST_MARYS
    {CASTLE_DRACULA, GALATZ, BUCHAREST, BELGRADE, SZEGED, BUDAPEST, END}, //KLAUSENBURG
    {BRUSSELS, PARIS, NANTEST, END}, //LE_HAVRE
    {BERLIN, NUREMBURG, FRANKFURT, COLOGNE, HAMBURG, END}, //LEIPZIG
    {SANTANDER, MADRID, LISBON, END}, //LISBON
    {MANCHESTER, SWANSEA, END}, //LIVERPOOL
    {MANCHESTER, SWANSEA, END}, //LONDON
    {SANTANDER, SARAGOSSA, ALICANTE, GRANADA, CADIZ, LISBON, END}, //MADRID
    {EDINBURGH, LONDON, LIVERPOOL, END}, //MANCHESTER
    {TOULOUSE, CLERMONT_FERRAND, GENEVA, ZURICH, MILAN, GENOA, END}, //MARSEILLES
    {ZURICH, MUNICH, VENICE, GENOA, MARSEILLES, END}, //MILAN
    {STRASBOURG, NUREMBURG, VIENNA, ZAGREB, VENICE, MILAN, ZURICH, END}, //MUNICH
    {LE HAVRE, PARIS, CLERMONT_FERRAND, BORDEAUX, END}, //NANTES
    {ROME, BARI, END}, //NAPLES
    {LEIPZIG, PRAGUE, MUNICH, STRASBOURG, FRANKFURT, END}, //NUREMBURG
    {BRUSSELS, STRASBOURG, GENEVA, CLERMONT_FERRAND, NANTES, LE_HAVRE, END}, //PARIS
    {LONDON, END}, //PLYMOUTH
    {BERLIN, VIENNA, NUREMBURG, END}, //PRAGUE
    {FLORENCE, BARI, NAPLES, END}, //ROME
    {VALONA, SOFIA, END}, //SALONICA
    {LISBON, SARAGOSSA, MADRID, END}, //SANTANDER
    {MADRID, SANTANDER, BORDEAUX, TOULOUSE, BARCELONA, ALICANTE, END}, //SARAGOSSA
    {ZAGREB, ST_JOSEPH_AND_ST_MARYS, BELGRADE, SOFIA, VALONA, END}, //SARAJEVO
    {BELGRADE, BUCHAREST, VARNA, SALONICA, SARAJEVO, END}, //SOFIA
    {PARIS, BRUSSELS, COLOGNE, FRANKFURT, NUREMBURG, MUNICH, ZURICH, GENEVA, END}, //STRASBOURG
    {LIVERPOOL, LONDON, END}, //SWANSEA
    {BUDAPEST, KLAUSENBURG, BELGRADE, ST_JOSEPH_AND_ST_MARY, ZAGREB, END}, //SZEGED
    {BORDEAUX, CLERMONT_FERRAND, MARSEILLES, BARCELONA, SARAGOSSA, END}, //TOULOUSE
    {SARAJEVO, SOFIA, SALONICA, ATHENS, END}, //VALONA
    {SOFIA, CONSTANTA, END}, //VARNA
    {FLORENCE, GENOA, MILAN, MUNICH, END}, //VENICE
    {PRAGUE, BUDAPEST, ZAGREB, MUNICH, END}, //VIENNA
    {MUNICH, VIENNA, BUDAPEST, SZEGED, ST_JOSEPH_AND_ST_MARYS, SARAJEVO, END}, //ZAGREB
    {STRASBOURG, MUNICH, MILAN, MARSEILLES, GENEVA, END} //ZURICH
    {END} // NORTH_SEA
    {END} // ENGLISH_CHANNEL
    {END} // IRISH_SEA
    {END} // ATLANTIC_OCEAN
    {END} // BAY_OF_BISCAY
    {END} // MEDITERRANEAN_SEA
    {END} // TYRRHENIAN_SEA
    {END} // IONIAN_SEA
    {END} // ADRIATIC_SEA
    {END} // BLACK_SEA
};

static const int adjacencyRail1[][NUM_MAP_LOCATIONS] = {
    {MADRID, BARCELONA, END}, // ALICANTE
    {END}, // AMSTERDAM
    {END}, // ATHENS
    {ALICANTE, SARAGOSSA, END}, // BARCELONA
    {NAPLES, END}, // BARI
    {SZEGED, SOFIA, END}, // BELGRADE
    {HAMBURG, PRAGUE, LEIPZIG, END}, // BERLIN
    {SARAGOSSA, PARIS, END}, // BORDEAUX
    {COLOGNE, PARIS, END}, // BRUSSELS
    {SZEGED, GALATZ, CONSTANTA, END}, // BUCHAREST
    {VIENNA, SZEGED, END}, // BUDAPEST
    {END}, // CADIZ
    {END}, // CAGLIARI
    {END}, // CASTLE_DRACULA
    {END}, // CLERMONT_FERRAND
    {BRUSSELS, FRANKFURT, END}, // COLOGNE
    {BUCHAREST, END}, // CONSTANTA
    {END}, // DUBLIN
    {MANCHESTER, END}, // EDINBURGH
    {MILAN, ROME, END}, // FLORENCE
    {COLOGNE, LEIPZIG, STRASBOURG, END}, // FRANKFURT
    {BUCHAREST, END}, // GALATZ
    {END}, // GALWAY
    {MILAN, END}, // GENEVA
    {MILAN, END}, // GENOA
    {END}, // GRANADA
    {BERLIN, END}, // HAMBURG
    {END}, // ST_JOSEPH_AND_ST_MARYS
    {END}, // KLAUSENBURG
    {PARIS , END}, // LE_HAVRE
    {BERLIN, FRANKFURT, NUREMBURG, END}, // LEIPZIG
    {MADRID, END}, // LISBON
    {MANCHESTER, END}, // LIVERPOOL
    {SWANSEA, MANCHESTER, END}, // LONDON
    {LISBON, SANTANDER, SARAGOSSA, ALICANTE, END}, // MADRID
    {EDINBURGH, LONDON, LIVERPOOL, END}, // MANCHESTER
    {PARIS, END}, // MARSEILLES
    {GENEVA, ZURICH, FLORENCE, GENOA, END}, // MILAN
    {NUREMBURG, END}, // MUNICH
    {END}, // NANTES
    {ROME, BARI, END}, // NAPLES
    {LEIPZIG, MUNICH, END}, // NUREMBURG
    {LE HAVRE, BRUSSELS, MARSEILLES, BORDEAUX, END}, // PARIS
    {END}, // PLYMOUTH
    {BERLIN, VIENNA, END}, // PRAGUE
    {FLORENCE, NAPLES, END}, // ROME
    {SOFIA, END}, // SALONICA
    {MADRID, END}, // SANTANDER
    {MADRID, BORDEAUX, BARCELONA, END}, // SARAGOSSA
    {END}, // SARAJEVO
    {SALONICA, BELGRADE, VARNA, END}, // SOFIA
    {FRANKFURT, ZURICH, END}, // STRASBOURG
    {LONDON, END}, // SWANSEA
    {BUDAPEST, BUCHAREST, BELGRADE, END}, // SZEGED
    {END}, // TOULOUSE
    {END}, // VALONA
    {SOFIA, END}, // VARNA
    {VIENNA, END}, // VENICE
    {VENICE, PRAGUE, BUDAPEST, END}, // VIENNA
    {END}, // ZAGREB
    {STRASBOURG, MILAN, END} // ZURICH
    {END} // NORTH_SEA
    {END} // ENGLISH_CHANNEL
    {END} // IRISH_SEA
    {END} // ATLANTIC_OCEAN
    {END} // BAY_OF_BISCAY
    {END} // MEDITERRANEAN_SEA
    {END} // TYRRHENIAN_SEA
    {END} // IONIAN_SEA
    {END} // ADRIATIC_SEA
    {END} // BLACK_SEA
};

/*
static const int *adjacencyRail2[] = {
    {, END}, // ALICANTE
    {, END}, // AMSTERDAM
    {, END}, // ATHENS
    {, END}, // BARCELONA
    {, END}, // BARI
    {, END}, // BELGRADE
    {, END}, // BERLIN
    {, END}, // BORDEAUX
    {, END}, // BRUSSELS
    {, END}, // BUCHAREST
    {, END}, // BUDAPEST
    {, END}, // CADIZ
    {, END}, // CAGLIARI
    {, END}, // CASTLE_DRACULA
    {, END}, // CLERMONT_FERRAND
    {, END}, // COLOGNE
    {, END}, // CONSTANTA
    {, END}, // DUBLIN
    {, END}, // EDINBURGH
    {, END}, // FLORENCE
    {, END}, // FRANKFURT
    {, END}, // GALATZ
    {, END}, // GALWAY
    {, END}, // GENEVA
    {, END}, // GENOA
    {, END}, // GRANADA
    {, END}, // HAMBURG
    {, END}, // ST_JOSEPH_AND_ST_MARYS
    {, END}, // KLAUSENBURG
    {, END}, // LE_HAVRE
    {, END}, // LEIPZIG
    {, END}, // LISBON
    {, END}, // LIVERPOOL
    {, END}, // LONDON
    {, END}, // MADRID
    {, END}, // MANCHESTER
    {, END}, // MARSEILLES
    {, END}, // MILAN
    {, END}, // MUNICH
    {, END}, // NANTES
    {, END}, // NAPLES
    {, END}, // NUREMBURG
    {, END}, // PARIS
    {, END}, // PLYMOUTH
    {, END}, // PRAGUE
    {, END}, // ROME
    {, END}, // SALONICA
    {, END}, // SANTANDER
    {, END}, // SARAGOSSA
    {, END}, // SARAJEVO
    {, END}, // SOFIA
    {, END}, // STRASBOURG
    {, END}, // SWANSEA
    {, END}, // SZEGED
    {, END}, // TOULOUSE
    {, END}, // VALONA
    {, END}, // VARNA
    {, END}, // VENICE
    {, END}, // VIENNA
    {, END}, // ZAGREB
    {, END} // ZURICH
    {END} // NORTH_SEA
    {END} // ENGLISH_CHANNEL
    {END} // IRISH_SEA
    {END} // ATLANTIC_OCEAN
    {END} // BAY_OF_BISCAY
    {END} // MEDITERRANEAN_SEA
    {END} // TYRRHENIAN_SEA
    {END} // IONIAN_SEA
    {END} // ADRIATIC_SEA
    {END} // BLACK_SEA

};

static const int *adjacencyRail3[] = {
    {, END}, // ALICANTE
    {, END}, // AMSTERDAM
    {, END}, // ATHENS
    {, END}, // BARCELONA
    {, END}, // BARI
    {, END}, // BELGRADE
    {, END}, // BERLIN
    {, END}, // BORDEAUX
    {, END}, // BRUSSELS
    {, END}, // BUCHAREST
    {, END}, // BUDAPEST
    {, END}, // CADIZ
    {, END}, // CAGLIARI
    {, END}, // CASTLE_DRACULA
    {, END}, // CLERMONT_FERRAND
    {, END}, // COLOGNE
    {, END}, // CONSTANTA
    {, END}, // DUBLIN
    {, END}, // EDINBURGH
    {, END}, // FLORENCE
    {, END}, // FRANKFURT
    {, END}, // GALATZ
    {, END}, // GALWAY
    {, END}, // GENEVA
    {, END}, // GENOA
    {, END}, // GRANADA
    {, END}, // HAMBURG
    {, END}, // ST_JOSEPH_AND_ST_MARYS
    {, END}, // KLAUSENBURG
    {, END}, // LE_HAVRE
    {, END}, // LEIPZIG
    {, END}, // LISBON
    {, END}, // LIVERPOOL
    {, END}, // LONDON
    {, END}, // MADRID
    {, END}, // MANCHESTER
    {, END}, // MARSEILLES
    {, END}, // MILAN
    {, END}, // MUNICH
    {, END}, // NANTES
    {, END}, // NAPLES
    {, END}, // NUREMBURG
    {, END}, // PARIS
    {, END}, // PLYMOUTH
    {, END}, // PRAGUE
    {, END}, // ROME
    {, END}, // SALONICA
    {, END}, // SANTANDER
    {, END}, // SARAGOSSA
    {, END}, // SARAJEVO
    {, END}, // SOFIA
    {, END}, // STRASBOURG
    {, END}, // SWANSEA
    {, END}, // SZEGED
    {, END}, // TOULOUSE
    {, END}, // VALONA
    {, END}, // VARNA
    {, END}, // VENICE
    {, END}, // VIENNA
    {, END}, // ZAGREB
    {, END} // ZURICH
    {END} // NORTH_SEA
    {END} // ENGLISH_CHANNEL
    {END} // IRISH_SEA
    {END} // ATLANTIC_OCEAN
    {END} // BAY_OF_BISCAY
    {END} // MEDITERRANEAN_SEA
    {END} // TYRRHENIAN_SEA
    {END} // IONIAN_SEA
    {END} // ADRIATIC_SEA
    {END} // BLACK_SEA

};
*/

static const int adjacencySea[][NUM_MAP_LOCATIONS] = {
    {MEDITERRANEAN_SEA, END}, // ALICANTE
    {NORTH_SEA, END}, // AMSTERDAM
    {IONIAN_SEA, END}, // ATHENS
    {MEDITERRANEAN_SEA, END}, // BARCELONA
    {ADRIATIC_SEA, END}, // BARI
    {END}, // BELGRADE
    {END}, // BERLIN
    {BAY_OF_BISCAY, END}, // BORDEAUX
    {END}, // BRUSSELS
    {END}, // BUCHAREST
    {END}, // BUDAPEST
    {ATLANTIC_OCEAN, END}, // CADIZ
    {MEDITERRANEAN_SEA, TYRRHENIAN_SEA, END}, // CAGLIARI
    {END}, // CASTLE_DRACULA
    {END}, // CLERMONT_FERRAND
    {END}, // COLOGNE
    {BLACK_SEA, END}, // CONSTANTA
    {IRISH_SEA, END}, // DUBLIN
    {NORTH_SEA, END}, // EDINBURGH
    {END}, // FLORENCE
    {END}, // FRANKFURT
    {END}, // GALATZ
    {ATLANTIC_OCEAN, END}, // GALWAY
    {END}, // GENEVA
    {TYRRHENIAN_SEA, END}, // GENOA
    {END}, // GRANADA
    {NORTH_SEA, END}, // HAMBURG
    {END}, // ST_JOSEPH_AND_ST_MARYS
    {END}, // KLAUSENBURG
    {ENGLISH_CHANNEL, END}, // LE_HAVRE
    {END}, // LEIPZIG
    {ATLANTIC_OCEAN, END}, // LISBON
    {IRISH_SEA, END}, // LIVERPOOL
    {ENGLISH_CHANNEL, END}, // LONDON
    {END}, // MADRID
    {END}, // MANCHESTER
    {MEDITERRANEAN_SEA, END}, // MARSEILLES
    {END}, // MILAN
    {END}, // MUNICH
    {BAY_OF_BISCAY, END}, // NANTES
    {TYRRHENIAN_SEA, END}, // NAPLES
    {END}, // NUREMBURG
    {END}, // PARIS
    {ENGLISH_CHANNEL, END}, // PLYMOUTH
    {END}, // PRAGUE
    {TYRRHENIAN_SEA, END}, // ROME
    {IONIAN_SEA, END}, // SALONICA
    {BAY_OF_BISCAY, END}, // SANTANDER
    {END}, // SARAGOSSA
    {END}, // SARAJEVO
    {END}, // SOFIA
    {END}, // STRASBOURG
    {IRISH_SEA, END}, // SWANSEA
    {END}, // SZEGED
    {END}, // TOULOUSE
    {IONIAN_SEA, END}, // VALONA
    {BLACK_SEA, END}, // VARNA
    {ADRIATIC_SEA, END}, // VENICE
    {END}, // VIENNA
    {END}, // ZAGREB
    {END} // ZURICH
    {EDINBURGH, AMSTERDAM, HAMBURG, ATLANTIC_OCEAN, END} // NORTH_SEA
    {PLYMOUTH, LONDON, LE_HAVRE, NORTH_SEA, ATLANTIC_OCEAN, END} // ENGLISH_CHANNEL
    {DUBLIN, LIVERPOOL, SWANSEA, ATLANTIC_OCEAN, END} // IRISH_SEA
    {GALWAY, LISBON, CADIZ, NORTH_SEA, IRISH_SEA, ENGLISH_CHANNEL, BAY_OF_BISCAY, MEDITERRANEAN_SEA, END} // ATLANTIC_OCEAN
    {NANTES, BORDEAUX, SANTANDER, ATLANTIC_OCEAN, END} // BAY_OF_BISCAY
    {ALICANTE, BARCELONA, MARSEILLES, CAGLIARI, ATLANTIC_OCEAN, TYRRHENIAN_SEA, END} // MEDITERRANEAN_SEA
    {CAGLIARI, ROME, NAPLES, MEDITERRANEAN_SEA, IONIAN_SEA, END} // TYRRHENIAN_SEA
    {VALONA, ATHENS, SALONICA, TYRRHENIAN_SEA, ADRIATIC_SEA, END} // IONIAN_SEA
    {VENICE, BARI, IONIAN_SEA, END} // ADRIATIC_SEA
    {CONSTANTA, VARNA, END} // BLACK_SEA
};

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
            if (realDest >= DOUBLE_BACK_1 && realDest <= DOUBLE_BACK_5) {
                int movesBack = realDest - DOUBLE_BACK_1;
                LocationID history[TRAIL_SIZE];
                getHistory(hunterView, curPlayer, history);
                
                realDest = history[movesBack + 1];
            }
        } else if (dest >= DOUBLE_BACK_1 && dest <= DOUBLE_BACK_5) {
            int movesBack = dest - DOUBLE_BACK_1;
            printf("%d\n", movesBack);
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
                printf("Dracula losing health\n");
                hunterView->players[curPlayer]->health -= 2;
            } else if (realDest == CASTLE_DRACULA) {
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
    
    // starting from the most reccent move
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
    LocationID result = malloc(NUM_MAP_LOCATIONS * sizeof(LocationID));
    int adjacentLocations = 0;
    int i;
    // include current city
    result[adjacentLocations] = from;
    adjacentLocations ++;
    for (i = 0; adjacentRoad[from][i] != END; i++) {
        result[adjacentLocations] = adjacentRoad[from][i];
        adjacentLocations++;
    }
    for (i = 0; adjacentSea[from][i] != END; i++) {
        result[adjacentLocations] = adjacentSea[from][i];
        adjacentLocations++;
    }
    if (player != PLAYER_DRACULA) {
        if (round % 4 == 1) {
            for (i = 0; adjacentRail1[from][i] != END; i++) {
                result[adjacentLocations] = adjacentRail1[from][i];
                adjacentLocations++;
            }
        } else if (round % 4 == 2) {
            for (i = 0; adjacentRail2[from][i] != END; i++) {
                result[adjacentLocations] = adjacentRail2[from][i];
                adjacentLocations++;
            }        
        } else if (round % 4 == 3) {
            for (i = 0; adjacentRail3[from][i] != END; i++) {
                result[adjacentLocations] = adjacentRail3[from][i];
                adjacentLocations++;
            }
        }
    }
    *numLocations = adjacentLocations;
    return result;
}


