// HunterView.h
// Official Interface for HunterViewADT
// COMP1927 13s2

#ifndef HUNTER_VIEW_H
#define HUNTER_VIEW_H

#include "cities.h"

#define FALSE 0
#define TRUE 1

// Players
#define PLAYER_LORD_GODALMING   0
#define PLAYER_DR_SEWARD        1
#define PLAYER_VAN_HELSING      2
#define PLAYER_MINA_HARKER      3
#define PLAYER_DRACULA          4

#define NUM_PLAYERS     5

#define TRAIL_SIZE 12

//Score
#define GAME_START_SCORE                366
#define SCORE_LOSS_DRACULA_TURN         1
#define SCORE_LOSS_HUNTER_HOSPITAL      6
#define SCORE_LOSS_VAMPIRE_MATURES      13

// Dracula Health
#define GAME_START_BLOOD_POINTS         40
#define LIFE_LOSS_HUNTER_ENCOUNTER      10
#define LIFE_LOSS_SEA                   2
#define LIFE_GAIN_CASTLE_DRACULA        10

//Hunter Health
#define GAME_START_HUNTER_LIFE_POINTS   9
#define LIFE_LOSS_TRAP_ENCOUNTER        2
#define LIFE_LOSS_DRACULA_ENCOUNTER     4
#define LIFE_GAIN_REST                  3 

     
typedef struct hunterView *HunterView;
     
typedef int PlayerID;
typedef int Round;
         

// newHunterView creates a new hunter view to summarise the current state of
// the game.
//
// pastPlays is a string of all the plays made in the game so far by all
// players (including Dracula) from earliest to most recent.
//
//
// messages is an array containing a playerMessage for each play in the game
// so far. It will have exactly the same number of elements as there are plays
// in pastPlays. The message from the first play will be at index 0, and so on.
// The contents of each playerMessage will be exactly as provided by the player.
//
// The "playerMessage" type is defined in game.h.
// You are free to ignore messages if you wish.
     
HunterView newHunterView (char *pastPlays, playerMessage messages[]);
     
     
// this function frees all memory previously allocated for the HunterView
// toBeDeleted. toBeDeleted should not be accessed after the call.
void disposeHunterView (HunterView toBeDeleted);

//Functions to return simple information about the current state of the game

//Get the current round
Round getRound (HunterView currentView);

//Get the id of current player - ie whose turn is it?
// Only returns a 'playerID' which is one of:
//   LORD_GODALMING (0): Lord Godalming's turn
//   DR_SEWARD      (1): Dr. Seward's turn
//   VAN_HELSING    (2): Van Helsing's turn
//   MINA_HARKER    (3): Mina Harker's turn
//   DRACULA        (4): Dracula's turn
PlayerID getCurrentPlayer (HunterView currentView);

//Get the current score
// Returns a positive integer [0...366]
int getScore(HunterView currentView);

//Get the current health points for a given player
// 'player' specifies which players's life/blood points to return
//    and must be a value in the interval [0...4] (see 'player' type)

int getHealth(HunterView currentView, PlayerID player);

// Get the current location id of a given player
// May be UNKNOWN_LOCATION if the player has not had a turn yet
// (ie at the beginning of the game when the round is 0)
// otherwise for a hunter it should be an integer in the interval [0..70] 
// The given roundNumber is >= 0 and <= the current round number
// 'whichHunter' specifies which hunter's location to return
//    and must be a value in the interval [0...3] (see 'player' type)
// Or for dracula it should 
// gets the location of Dracula at the start of a particular round
// Returns an integer:
//   in the interval [0...70] if Dracula was known to be in a city or sea
//   CITY_UNKNOWN     if Dracula was in an unknown city
//   SEA_UNKNOWN      if Dracula was in an unknown sea
//   HIDE             if Dracula was known to have made a hide move
//   DOUBLE_BACK_N    where N is [0...5], if Dracula was known to have 
//                    made a double back move N positions back in the trail
//                    e.g. DOUBLE_BACK_1 is the last place place he visited
//   TELEPORT         if Dracula apparated back to Castle Dracula
//   LOCATION_UNKNOWN if the round number is 0
LocationID getLocation(HunterView currentView, PlayerID player);     

//Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns for the given player
// For dracula this may include integers:
//   in the interval [0...70] if Dracula was known to be in a city or sea
//   CITY_UNKNOWN     if Dracula was in an unknown city
//   SEA_UNKNOWN      if Dracula was in an unknown sea

//   HIDE             if Dracula was known to have made a hide move
//   DOUBLE_BACK_N    where N is [0...5], if Dracula was known to have
//                    made a double back move N positions back in the trail
//                    e.g. DOUBLE_BACK_1 is the last place place he visited
//   TELEPORT         if Dracula apparated back to Castle Dracula


// For any player if the move does not exist yet (i.e, the start of the game),
// the value should be UNKNOWN_LOCATION (-1)
// For example after 2 turns the array may have the contents
// {29, 182, -1, -1, -1, -1} 
// This would mean in the first move the player started on location 182 
// then moved to the current location of 29
void getHistory (HunterView currentView, PlayerID player,LocationID trail[TRAIL_SIZE]);

//Functions that query the map to find information about connectivity

//This function returns an array of LocationID that represent all locations that are connected 
//to the given LocationID. 
//road, rail and sea are connections should only be considered if the road, rail, sea parameters 
//are TRUE.
//The size of the array should be stored in the variable pointed to by numLocations
//The array can be in any order but must contain unique entries
//Your function must take into account the round and player id for rail travel
//Your function must take into account that dracula can't move to the hospital or travel by rail
//but need not take into account draculas trail
//The destination 'from' should be included.
LocationID * connectedLocations(HunterView currentView, int * numLocations, LocationID from, 
                              PlayerID player, Round round, int road, int rail, int sea);


#endif
