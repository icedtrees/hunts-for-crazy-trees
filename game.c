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

//#define printf(...)
#define getchar(...)

typedef struct _node *Node;
typedef struct _node {
    LocationID move;
    LocationID realLoc;
    int knownToHunter;
    int encounters;
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

struct gameView {
    Round curRound;
    PlayerID curPlayer;
    int curScore;
    Player players[NUM_PLAYERS];
    // Keep track of traps and vampires on the map
    // +1 per trap and +10 for a vampire
    int obstacles[NUM_MAP_LOCATIONS];
    char pastPlays[100000];
    char revealedPastPlays[100000];
    int vampireCountdown;
    HunterView hView;
};

// Global GameView
GameView g;
char registeredPlay[3];

static Node newNode(LocationID move) {
    Node n = malloc(sizeof(node));
    n->move = move;
    n->knownToHunter = FALSE;
    n->encounters = 0;
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

GameView newGameView() {
    GameView gView = malloc(sizeof(struct gameView));
    playerMessage messages[] = {};
    gView->pastPlays[0] = 0;
    gView->hView = newHunterView(gView->pastPlays, messages);
    gView->curRound = 0;
    gView->curScore = 366;
    gView->curPlayer = 0;
    gView->vampireCountdown = -1;
    
    int i;
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        gView->obstacles[i] = 0;
    }
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i == PLAYER_DRACULA) {
            gView->players[i] = newPlayer(GAME_START_BLOOD_POINTS);
        } else {
            gView->players[i] = newPlayer(GAME_START_HUNTER_LIFE_POINTS);
        }
    }
    
    return gView;
}

void confirmBestPlay();
void disposeGameView(void);

int main(int argc, char **argv) {
    int seed;
    if (argc > 1) {
        sscanf(argv[1], "%d", &seed);
        srand(seed);
    } else {
        srand(time(NULL));
    }
    // Create a new empty GameView
    g = newGameView();
    
    // While the game isn't over yet
    while (g->hView->curScore > 0 && g->hView->players[PLAYER_DRACULA]->health > 0) {
        printf("GAME: A new turn! enter to continue..\n");
        fflush(stdout);
        getchar();
        if (getCurrentPlayer(g->hView) < PLAYER_DRACULA) {
            // Current player is a hunter
            decideMove(g->hView);
            confirmBestPlay();
        } else {
            // IT'S DRACULA AAA
            //registerBestPlay("CD", "");
            decideMoveDracula(g, seed);
            confirmBestPlay();
        }
        printf("Score: %d, Dracula's health: %d\n", g->hView->curScore, g->hView->players[PLAYER_DRACULA]->health);
        printf("%s\n", g->pastPlays);
    }
    
    if (g->hView->curScore <= 0) {
        printf("Hunters lost =[\n");
    } else if (g->hView->players[PLAYER_DRACULA]->health <= 0) {
        printf("Hunters won with %d points!\n", g->hView->curScore);
    } else {
        printf("I don't know what happened but good game everyone\n");
    }
    
    int finalScore = g->hView->curScore;
    fprintf(stderr, "%s\n", g->revealedPastPlays);
    disposeGameView();
    
    fprintf(stdout, "%d\n", finalScore);
    return EXIT_SUCCESS;
}

void disposeGameView(void) {
    int i;
    Node previousMove = NULL;
    Node currentMove = NULL;
    for (i = 0; i < NUM_PLAYERS; i ++) {
        currentMove = g->players[i]->moves->first;
        while (currentMove != NULL) {
            previousMove = currentMove;
            currentMove = currentMove->next;
            free(previousMove);
        }
        free(g->players[i]->moves);
        free(g->players[i]);
    }
    disposeHunterView(g->hView);
    free(g);
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

static void generatePastPlays(void) {
    g->pastPlays[0] = 0;
    Round round;
    Node curMove[NUM_PLAYERS];
    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        curMove[i] = g->players[i]->moves->first;
    }
    for (round = 0; round <= g->curRound; round++) {
        for (i = 0; i < NUM_PLAYERS; i++) {
            if (curMove[i] == NULL) {
                // remove trailing space
                g->pastPlays[strlen(g->pastPlays) - 1] = 0;
                return;
            }
            if (i == PLAYER_LORD_GODALMING) {
                strcat(g->pastPlays, "G");
            } else if (i == PLAYER_DR_SEWARD) {
                strcat(g->pastPlays, "S");
            } else if (i == PLAYER_VAN_HELSING) {
                strcat(g->pastPlays, "H");
            } else if (i == PLAYER_MINA_HARKER) {
                strcat(g->pastPlays, "M");
            } else if (i == PLAYER_DRACULA) {
                strcat(g->pastPlays, "D");
            }
            
            if (curMove[i]->move >= HIDE) {
                strcat(g->pastPlays, names[curMove[i]->move]);
            } else if (curMove[i]->knownToHunter) {
                strcat(g->pastPlays, names[curMove[i]->move]);
            } else if (!curMove[i]->knownToHunter) {
                if (curMove[i]->move <= ZURICH) {
                    strcat(g->pastPlays, "C?");
                } else {
                    strcat(g->pastPlays, "S?");
                }
            }
            int encounters = curMove[i]->encounters;
            if (encounters >= 100) {
                strcat(g->pastPlays, "D... ");
                encounters -= 100;
            } else {
                strcat(g->pastPlays, ".... ");
            }
            
            curMove[i] = curMove[i]->next;
        }
    }
}

static void generateRevealedPastPlays(void) {
    g->revealedPastPlays[0] = 0;
    Round round;
    Node curMove[NUM_PLAYERS];
    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        curMove[i] = g->players[i]->moves->first;
    }
    for (round = 0; round <= g->curRound; round++) {
        for (i = 0; i < NUM_PLAYERS; i++) {
            if (curMove[i] == NULL) {
                // remove trailing space
                g->revealedPastPlays[strlen(g->revealedPastPlays) - 1] = 0;
                return;
            }
            if (i == PLAYER_LORD_GODALMING) {
                strcat(g->revealedPastPlays, "G");
            } else if (i == PLAYER_DR_SEWARD) {
                strcat(g->revealedPastPlays, "S");
            } else if (i == PLAYER_VAN_HELSING) {
                strcat(g->revealedPastPlays, "H");
            } else if (i == PLAYER_MINA_HARKER) {
                strcat(g->revealedPastPlays, "M");
            } else if (i == PLAYER_DRACULA) {
                strcat(g->revealedPastPlays, "D");
            }
            
            strcat(g->revealedPastPlays, names[curMove[i]->move]);

            int encounters = curMove[i]->encounters;
            if (encounters >= 100) {
                strcat(g->revealedPastPlays, "D... ");
                encounters -= 100;
            } else {
                strcat(g->revealedPastPlays, ".... ");
            }
            
            curMove[i] = curMove[i]->next;
        }
    }
}
        

static void revealDracula(int turnsAgo) {
    Player dracula = g->players[PLAYER_DRACULA];
    Node curr = dracula->moves->last;
    int i;
    for (i = 0; i < turnsAgo; i++) {
        curr = curr->prev;
    }
    curr->knownToHunter = TRUE;
}

static int inPath(LocationID location) {
    Player dracula = g->players[PLAYER_DRACULA];
    Node curr = dracula->moves->last;
    int i;
    for (i = 0; i < TRAIL_SIZE && curr != NULL; i++) {
        if (curr->move == location) {
            return i;
        }
        curr = curr->prev;
    }
    return -1;
}

LocationID history(Player player, int turnsAgo) {
    Node curr = player->moves->last;
    int i;
    for (i = 0; i < turnsAgo; i++) {
        curr = curr->prev;
    }
    return curr->move;
}

void updateGame(char *play) {
    LocationID playID = moveID(play[0], play[1]);
    Player player = g->players[g->curPlayer];
    if (g->curPlayer != PLAYER_DRACULA) {
        // add the move
        addMove(player->moves, playID);
        player->curLoc = playID;
        player->moves->last->knownToHunter = TRUE;
        
        // what's the REAL move? ie where are they REALLY
        // it's a hunter so it's exactly what they register
        player->moves->last->realLoc = playID;
        
        // check if found dracula's path
        int turnsAgo = inPath(playID);
        if (turnsAgo != -1) {
            // in the path
            revealDracula(turnsAgo);
        }
        
        // check if any actions
        if (g->obstacles[playID] > 10) {
            // encounter immature vamper
            g->obstacles[playID] -= 10;
            g->vampireCountdown = -1;
            player->moves->last->encounters += 10;
        }
        while (g->obstacles[playID] > 0 && player->health > 0) {
            // encounter trap
            g->obstacles[playID]--;
            player->health -= 2;
            player->moves->last->encounters += 1;
        }
        printf("Dracula is at %s\n", names[g->players[PLAYER_DRACULA]->curLoc]);
        if (g->players[PLAYER_DRACULA]->curLoc == playID) {
            // found dracula
            printf("HUNTER HITS DRACULA REALLY HARD\n");
            if (player->health > 0) {
                g->players[PLAYER_DRACULA]->health -= 10;
            }
            player->health -= 4;
            player->moves->last->encounters += 100;
            revealDracula(0);
        }
        if (player->health <= 0) {
            //dead
            player->health = 9;
            player->curLoc = ST_JOSEPH_AND_ST_MARYS;
            g->curScore -= 6;
        }
        
        // did he rest
        if (playID == player->curLoc) {
            player->health += 3;
        }

        if (g->curPlayer == PLAYER_DRACULA - 1 && g->curRound > 6) {
            // reveal dracula if hunters research
            int numHuntersResearched = 0;
            int j;
            for (j = 0; j < PLAYER_DRACULA; j++) {
                if (g->players[j]->curLoc == g->players[j]->moves->last->prev->move) {
                    numHuntersResearched++;
                }
            }
            if (numHuntersResearched == 4) {
                printf("HUNTERS RESEARCHED, REVEAL DRACULA\n");
                revealDracula(6);
            }
        }
    } else {
        //dracula
        // add the move
        addMove(player->moves, playID);
        
        // what's the REAL move? ie where are they REALLY
        // it's a hunter so it's exactly what they register
        LocationID realMove;
        if (playID <= BLACK_SEA) {
            // real city, no change
            realMove = playID;
        } else if (playID == HIDE) {
            realMove = history(player, 1);
        } else if (playID == DOUBLE_BACK_1) {
            realMove = history(player, 1);
        } else if (playID == DOUBLE_BACK_2) {
            realMove = history(player, 2);
        } else if (playID == DOUBLE_BACK_3) {
            realMove = history(player, 3);
        } else if (playID == DOUBLE_BACK_4) {
            realMove = history(player, 4);
        } else if (playID == DOUBLE_BACK_5) {
            realMove = history(player, 5);
        } else if (playID == TELEPORT) {
            realMove = CASTLE_DRACULA;
        }
        player->moves->last->realLoc = realMove;
        player->curLoc = realMove;
        if (realMove == CASTLE_DRACULA) {
            player->moves->last->knownToHunter = TRUE;
        }
        
        // run into hunter?
        int j;
        for (j = 0; j < PLAYER_DRACULA; j++) {
            if (g->players[j]->curLoc == realMove) {
                printf("DRACULA RAN INTO A HUNTER =o\n");
                player->moves->last->knownToHunter = TRUE;
            }
        }
        
        // check for any actions
        if (g->curRound > 0 && g->curRound % 13 == 0) {
            // multiple of 13, releave a vampire
            g->obstacles[realMove] += 10;
            g->vampireCountdown = 6;
        }
        
        // increment game things since end of round
        g->curRound++;
        g->curScore--;
    }
    // increment
    g->curPlayer = (g->curPlayer + 1) % NUM_PLAYERS;
    if (g->vampireCountdown > 0) {
        g->vampireCountdown--;
    }
    if (g->vampireCountdown == 0) {
        g->curScore -= 13;
        g->vampireCountdown = -1;
    }
    
    generatePastPlays();
    generateRevealedPastPlays();
}

void confirmBestPlay() {
    printf("GAME: player %d confirmed move as %s!\n", g->curPlayer, registeredPlay);
    // Free up the old hunter view
    disposeHunterView(g->hView);
    printf("    : disposed old hunterview\n");
    
    // Register the new play
    updateGame(registeredPlay);
    printf("    : registered new play\n");
    
    // Create a new hunterView
    playerMessage messages[] = {};
    g->hView = newHunterView(g->pastPlays, messages);
    printf("    : created new hunterview\n");
}

void registerBestPlay(char *play, playerMessage message) {
    printf("GAME: player %d registered %s!\n", g->curPlayer, play);
    strcpy(registeredPlay, play);
    printf("    : saved!\n");
}

LocationID gameGetLocation(GameView gView, PlayerID player) {
    return gView->players[player]->curLoc;
}

HunterView gameGetHunterView(GameView gView) {
    return gView->hView;
}

void gameGetHistory(GameView gView, PlayerID player, LocationID trail[TRAIL_SIZE]) {
    int i;
    // set all to UNKNOWN_LOCATION (-1) first
    for (i = 0; i < TRAIL_SIZE; i++) {
        trail[i] = UNKNOWN_LOCATION;
    }
    
    // starting from the most recent move
    MoveList moves = gView->players[player]->moves;
    Node curMove = moves->last;
    
    i = 0;
    while (curMove != NULL && i < TRAIL_SIZE) {
        trail[i] = curMove->realLoc;
        i++;
        curMove = curMove->prev;
    }
}
