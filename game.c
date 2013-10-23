#include "game.h"
#include "HunterView.h"
#include "hunter.h"
//#include "dracula.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

typedef struct _gameView *GameView;
typedef struct _gameView {
    Player players[NUM_PLAYERS];
    HunterView hView;
} gameView;

// Global GameView and pastPlays string
GameView g;
char realPlays[100000];
char pastPlays[100000];

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

GameView newGameView() {
    GameView gView = malloc(sizeof(gameView));
    playerMessage messages[] = {};
    realPlays[0] = 0;
    pastPlays[0] = 0;
    gView->hView = newHunterView(pastPlays, messages);
    
    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i == PLAYER_DRACULA) {
            gView->players[i] = newPlayer(GAME_START_BLOOD_POINTS);
        } else {
            gView->players[i] = newPlayer(GAME_START_HUNTER_LIFE_POINTS);
        }
    }
    
    return gView;
}

int main(int argc, char **argv) {
    // Create a new empty GameView
    g = newGameView();
    
    // While the game isn't over yet
    while (g->hView->curScore > 0 && g->hView->players[PLAYER_DRACULA]->health > 0) {
        if (getCurrentPlayer(g->hView) < PLAYER_DRACULA) {
            // Current player is a hunter
            decideMove(g->hView);
        } else {
            // IT'S DRACULA AAA
            //decideMoveDracula(g);
        }
    }
    
    if (g->hView->curScore <= 0) {
        printf("Hunters lost =[\n");
    } else if (g->hView->players[PLAYER_DRACULA]->health <= 0) {
        printf("Hunters won with %d points!\n", g->hView->curScore);
    } else {
        printf("I don't know what happened but good game everyone\n");
    }
    
    return EXIT_SUCCESS;
}

void updateRealPlays(char *realPlays, char *play) {
    strcat(realPlays, " ");
    PlayerID curPlayer = getCurrentPlayer(g->hView);
    if (curPlayer == PLAYER_LORD_GODALMING) {
        strcat(realPlays, "G");
    } else if (curPlayer == PLAYER_DR_SEWARD) {
        strcat(realPlays, "S");
    } else if (curPlayer == PLAYER_VAN_HELSING) {
        strcat(realPlays, "H");
    } else if (curPlayer == PLAYER_MINA_HARKER) {
        strcat(realPlays, "J");
    } else if (curPlayer == PLAYER_DRACULA) {
        strcat(realPlays, "D");
    }
    strcat(realPlays, play);
    char actions[5];
    actions[0] = 0;
    
    

void registerBestPlay(char *play, playerMessage message) {
    // Free up the old hunter view
    disposeHunterView(g->hView);
    
    // Register the new play
    updateRealPlays(realPlays, play);
    
    // Change to a hidden past plays string for hunters
    convertPlays(realPlays, pastPlays);
    
    // Create a new hunterView
    playerMessage messages[] = {};
    g->hView = newHunterView(pastPlays, messages);
}
