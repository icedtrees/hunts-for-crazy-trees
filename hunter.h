// hunter.h
// Interface to your Fury of Dracula hunter AI
// By: TeamDracula
// Date: 1 January 2012
// Version: 1.0
     
void decideMove (HunterView gameState);    // hunter.h


// For testing purposes only - remove later
int shortestPath(HunterView hView, LocationID source, LocationID dest, LocationID **path);
void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths);
LocationID **getDraculaTrails(int histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail);