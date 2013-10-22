#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include "Queue.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define MAX_MESSAGE_SIZE 128

// When dracula is a in a location, he can travel to a maximum of eight other connected locations
#define MAX_ADJACENT_LOCATIONS 8

static const char names[NUM_LOCATIONS][3] = {
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
};

static const int primes[26] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 73, 79, 83, 89, 97, 101, 103};
static const int id[323] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 55, -1, -1, 3, -1, 33, -1, -1, -1, 52, -1, 11, -1, -1, -1, -1, -1, -1, 65, -1, -1, 57, -1, 19, -1, -1, -1, -1, -1, -1, -1, 5, -1, -1, -1, -1, -1, 32, -1, 37, -1, -1, -1, -1, 60, -1, 56, -1, -1, 45, -1, -1, -1, -1, 15, -1, 38, -1, -1, 21, 50, -1, 31, -1, -1, -1, 9, 26, -1, -1, -1, 54, -1, 17, -1, -1, 24, -1, -1, -1, 59, -1, -1, -1, -1, -1, -1, 68, -1, -1, -1, -1, -1, -1, -1, -1, 36, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, -1, -1, -1, -1, -1, -1, 28, -1, 51, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 49, -1, 10, -1, -1, -1, -1, -1, 12, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, 34, -1, -1, 7, -1, -1, -1, -1, 39, -1, 47, 67, -1, -1, -1, -1, -1, 6, -1, 44, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, -1, -1, 61, -1, -1, 14, -1, 16, -1, 53, -1, 43, -1, -1, -1, -1, 63, -1, 42, -1, -1, -1, 18, -1, -1, 64, -1, -1, -1, -1, 70, -1, -1, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, 20, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 69, -1, -1, -1, -1, 66, -1, -1, -1, -1, -1, 46, -1, -1, -1, 25, 41, 48, 62, -1, -1, 29, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 22, -1, -1, -1, -1, -1, 4, -1, -1, -1, 27, -1, -1, 58, -1, -1, -1, -1, -1, 40, -1, -1, -1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2};

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
    {CONSTANTA, GALATZ, SOFIA, BELGRADE, KLAUSENBURG, END}, //BUCHAREST
    {KLAUSENBURG, SZEGED, VIENNA, ZAGREB, END}, //BUDAPEST
    {GRANADA, MADRID, LISBON, END}, //CADIZ
    {END}, //CAGLIARI
    {GALATZ, KLAUSENBURG, END}, //CASTLE_DRACULA
    {PARIS, GENEVA, MARSEILLES, TOULOUSE, BORDEAUX, NANTES, END}, //CLERMONT_FERRAND
    {AMSTERDAM, HAMBURG, LEIPZIG, FRANKFURT, STRASBOURG, BRUSSELS, END}, //COLOGNE
    {GALATZ, BUCHAREST, VARNA, END}, //CONSTANTA
    {GALWAY, END}, //DUBLIN
    {MANCHESTER, END}, //EDINBURGH
    {VENICE, ROME, GENOA, END}, //FLORENCE
    {COLOGNE, LEIPZIG, NUREMBURG, STRASBOURG, END}, //FRANKFURT
    {CASTLE_DRACULA, CONSTANTA, BUCHAREST, KLAUSENBURG, END}, //GALATZ
    {DUBLIN, END}, //GALWAY
    {STRASBOURG, ZURICH, MARSEILLES, CLERMONT_FERRAND, PARIS, END}, //GENEVA
    {MILAN, VENICE, FLORENCE, MARSEILLES, END}, //GENOA
    {MADRID, ALICANTE, CADIZ, END}, //GRANADA
    {BERLIN, LEIPZIG, COLOGNE, END}, //HAMBURG
    {SZEGED, BELGRADE, SARAJEVO, ZAGREB, END}, //ST_JOSEPH_AND_ST_MARYS
    {CASTLE_DRACULA, GALATZ, BUCHAREST, BELGRADE, SZEGED, BUDAPEST, END}, //KLAUSENBURG
    {BRUSSELS, PARIS, NANTES, END}, //LE_HAVRE
    {BERLIN, NUREMBURG, FRANKFURT, COLOGNE, HAMBURG, END}, //LEIPZIG
    {SANTANDER, MADRID, CADIZ, END}, //LISBON
    {MANCHESTER, SWANSEA, END}, //LIVERPOOL
    {MANCHESTER, SWANSEA, PLYMOUTH, END}, //LONDON
    {SANTANDER, SARAGOSSA, ALICANTE, GRANADA, CADIZ, LISBON, END}, //MADRID
    {EDINBURGH, LONDON, LIVERPOOL, END}, //MANCHESTER
    {TOULOUSE, CLERMONT_FERRAND, GENEVA, ZURICH, MILAN, GENOA, END}, //MARSEILLES
    {ZURICH, MUNICH, VENICE, GENOA, MARSEILLES, END}, //MILAN
    {STRASBOURG, NUREMBURG, VIENNA, ZAGREB, VENICE, MILAN, ZURICH, END}, //MUNICH
    {LE_HAVRE, PARIS, CLERMONT_FERRAND, BORDEAUX, END}, //NANTES
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
    {BELGRADE, BUCHAREST, VARNA, SALONICA, VALONA, SARAJEVO, END}, //SOFIA
    {PARIS, BRUSSELS, COLOGNE, FRANKFURT, NUREMBURG, MUNICH, ZURICH, GENEVA, END}, //STRASBOURG
    {LIVERPOOL, LONDON, END}, //SWANSEA
    {BUDAPEST, KLAUSENBURG, BELGRADE, ST_JOSEPH_AND_ST_MARYS, ZAGREB, END}, //SZEGED
    {BORDEAUX, CLERMONT_FERRAND, MARSEILLES, BARCELONA, SARAGOSSA, END}, //TOULOUSE
    {SARAJEVO, SOFIA, SALONICA, ATHENS, END}, //VALONA
    {SOFIA, CONSTANTA, END}, //VARNA
    {FLORENCE, GENOA, MILAN, MUNICH, END}, //VENICE
    {PRAGUE, BUDAPEST, ZAGREB, MUNICH, END}, //VIENNA
    {MUNICH, VIENNA, BUDAPEST, SZEGED, ST_JOSEPH_AND_ST_MARYS, SARAJEVO, END}, //ZAGREB
    {STRASBOURG, MUNICH, MILAN, MARSEILLES, GENEVA, END}, //ZURICH
    {END}, // NORTH_SEA
    {END}, // ENGLISH_CHANNEL
    {END}, // IRISH_SEA
    {END}, // ATLANTIC_OCEAN
    {END}, // BAY_OF_BISCAY
    {END}, // MEDITERRANEAN_SEA
    {END}, // TYRRHENIAN_SEA
    {END}, // IONIAN_SEA
    {END}, // ADRIATIC_SEA
    {END}, // BLACK_SEA
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
    {PARIS, END}, // LE_HAVRE
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
    {LE_HAVRE, BRUSSELS, MARSEILLES, BORDEAUX, END}, // PARIS
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
    {STRASBOURG, MILAN, END}, // ZURICH
    {END}, // NORTH_SEA
    {END}, // ENGLISH_CHANNEL
    {END}, // IRISH_SEA
    {END}, // ATLANTIC_OCEAN
    {END}, // BAY_OF_BISCAY
    {END}, // MEDITERRANEAN_SEA
    {END}, // TYRRHENIAN_SEA
    {END}, // IONIAN_SEA
    {END}, // ADRIATIC_SEA
    {END}, // BLACK_SEA
};


static const int adjacencyRail2[][NUM_MAP_LOCATIONS] = {
    {MADRID, LISBON, BARCELONA, SARAGOSSA, SANTANDER, END}, // ALICANTE
    {END}, // AMSTERDAM
    {END}, // ATHENS
    {MADRID, BORDEAUX, ALICANTE, SARAGOSSA, END}, // BARCELONA
    {ROME, NAPLES, END}, // BARI
    {BUCHAREST, BUDAPEST, SZEGED, SOFIA, SALONICA, VARNA, END}, // BELGRADE
    {PRAGUE, LEIPZIG, NUREMBURG, HAMBURG, FRANKFURT, VIENNA, END}, // BERLIN
    {PARIS, MADRID, BARCELONA, MARSEILLES, SARAGOSSA, LE_HAVRE, BRUSSELS, END}, // BORDEAUX
    {PARIS, MARSEILLES, COLOGNE, LE_HAVRE, BORDEAUX, FRANKFURT, END}, // BRUSSELS
    {BUDAPEST, SZEGED, BELGRADE, GALATZ, CONSTANTA, END}, // BUCHAREST
    {BUCHAREST, BELGRADE, SZEGED, PRAGUE, VENICE, VIENNA, END}, // BUDAPEST
    {END}, // CADIZ
    {END}, // CAGLIARI
    {END}, // CASTLE_DRACULA
    {END}, // CLERMONT_FERRAND
    {STRASBOURG, PARIS, LEIPZIG, BRUSSELS, FRANKFURT, END}, // COLOGNE
    {BUCHAREST, GALATZ, SZEGED, END}, // CONSTANTA
    {END}, // DUBLIN
    {LONDON, LIVERPOOL, MANCHESTER, END}, // EDINBURGH
    {GENEVA, ZURICH, MILAN, ROME, GENOA, NAPLES, END}, // FLORENCE
    {STRASBOURG, ZURICH, LEIPZIG, NUREMBURG, COLOGNE, BERLIN, BRUSSELS, END}, // FRANKFURT
    {BUCHAREST, CONSTANTA, SZEGED, END}, // GALATZ
    {END}, // GALWAY
    {MILAN, GENOA, FLORENCE, ZURICH, END}, // GENEVA
    {MILAN, FLORENCE, GENEVA, ZURICH, END}, // GENOA
    {END}, // GRANADA
    {BERLIN, LEIPZIG, PRAGUE, END}, // HAMBURG
    {END}, // ST_JOSEPH_AND_ST_MARYS
    {END}, // KLAUSENBURG
    {PARIS, BORDEAUX, BRUSSELS, MARSEILLES, END}, // LE_HAVRE
    {STRASBOURG, PRAGUE, NUREMBURG, MUNICH, COLOGNE, BERLIN, HAMBURG, FRANKFURT, END}, // LEIPZIG
    {MADRID, SARAGOSSA, ALICANTE, SANTANDER, END}, // LISBON
    {EDINBURGH, LONDON, MANCHESTER, END}, // LIVERPOOL
    {MANCHESTER, LIVERPOOL, SWANSEA, EDINBURGH, END}, // LONDON
    {LISBON, BARCELONA, SARAGOSSA, ALICANTE, SANTANDER, BORDEAUX, END}, // MADRID
    {LIVERPOOL, EDINBURGH, LONDON, SWANSEA, END}, // MANCHESTER
    {PARIS, BORDEAUX, BRUSSELS, LE_HAVRE, END}, // MARSEILLES
    {GENEVA, STRASBOURG, ZURICH, ROME, GENOA, FLORENCE, END}, // MILAN
    {NUREMBURG, LEIPZIG, END}, // MUNICH
    {END}, // NANTES
    {BARI, ROME, FLORENCE, END}, // NAPLES
    {BERLIN, LEIPZIG, FRANKFURT, MUNICH, END}, // NUREMBURG
    {MARSEILLES, COLOGNE, SARAGOSSA, LE_HAVRE, BORDEAUX, BRUSSELS, END}, // PARIS
    {END}, // PLYMOUTH
    {VENICE, LEIPZIG, BUDAPEST, BERLIN, HAMBURG, VIENNA, END}, // PRAGUE
    {BARI, MILAN, FLORENCE, NAPLES, END}, // ROME
    {VARNA, SOFIA, BELGRADE, END}, // SALONICA
    {MADRID, SARAGOSSA, ALICANTE, LISBON, END}, // SANTANDER
    {PARIS, MADRID, LISBON, BARCELONA, ALICANTE, SANTANDER, BORDEAUX, END}, // SARAGOSSA
    {END}, // SARAJEVO
    {VARNA, SALONICA, SZEGED, BELGRADE, END}, // SOFIA
    {ZURICH, LEIPZIG, COLOGNE, MILAN, FRANKFURT, END}, // STRASBOURG
    {MANCHESTER, LONDON, END}, // SWANSEA
    {BUCHAREST, BUDAPEST, BELGRADE, GALATZ, SOFIA, CONSTANTA, VIENNA, END}, // SZEGED
    {END}, // TOULOUSE
    {END}, // VALONA
    {SOFIA, SALONICA, BELGRADE, END}, // VARNA
    {BUDAPEST, PRAGUE, VIENNA, END}, // VENICE
    {BUDAPEST, SZEGED, PRAGUE, VENICE, BERLIN, END}, // VIENNA
    {END}, // ZAGREB
    {STRASBOURG, GENEVA, FRANKFURT, MILAN, GENOA, FLORENCE, END}, // ZURICH
    {END}, // NORTH_SEA
    {END}, // ENGLISH_CHANNEL
    {END}, // IRISH_SEA
    {END}, // ATLANTIC_OCEAN
    {END}, // BAY_OF_BISCAY
    {END}, // MEDITERRANEAN_SEA
    {END}, // TYRRHENIAN_SEA
    {END}, // IONIAN_SEA
    {END}, // ADRIATIC_SEA
    {END} // BLACK_SEA
};

static const int adjacencyRail3[][NUM_MAP_LOCATIONS] = {
    {MADRID, LISBON, BARCELONA, SARAGOSSA, SANTANDER, BORDEAUX, END}, // ALICANTE
    {END}, // AMSTERDAM
    {END}, // ATHENS
    {PARIS, MADRID, LISBON, SARAGOSSA, ALICANTE, SANTANDER, BORDEAUX, END}, // BARCELONA
    {ROME, FLORENCE, NAPLES, END}, // BARI
    {BUCHAREST, BUDAPEST, SZEGED, GALATZ, SOFIA, CONSTANTA, SALONICA, VARNA, VIENNA, END}, // BELGRADE
    {VENICE, STRASBOURG, PRAGUE, LEIPZIG, BUDAPEST, MUNICH, COLOGNE, HAMBURG, NUREMBURG, FRANKFURT, VIENNA, END}, // BERLIN
    {PARIS, SANTANDER, BARCELONA, MARSEILLES, COLOGNE, SARAGOSSA, ALICANTE, LE_HAVRE, LISBON, BRUSSELS, MADRID, END}, // BORDEAUX
    {STRASBOURG, PARIS, LEIPZIG, MARSEILLES, COLOGNE, SARAGOSSA, LE_HAVRE, BORDEAUX, FRANKFURT, END}, // BRUSSELS
    {BUDAPEST, BELGRADE, SZEGED, GALATZ, SOFIA, CONSTANTA, VIENNA, END}, // BUCHAREST
    {BUCHAREST, BELGRADE, SZEGED, GALATZ, PRAGUE, SOFIA, CONSTANTA, VENICE, BERLIN, VIENNA, END}, // BUDAPEST
    {END}, // CADIZ
    {END}, // CAGLIARI
    {END}, // CASTLE_DRACULA
    {END}, // CLERMONT_FERRAND
    {STRASBOURG, ZURICH, PARIS, LEIPZIG, NUREMBURG, MARSEILLES, BERLIN, LE_HAVRE, BORDEAUX, BRUSSELS, FRANKFURT, END}, // COLOGNE
    {BUCHAREST, BUDAPEST, SZEGED, BELGRADE, GALATZ, END}, // CONSTANTA
    {END}, // DUBLIN
    {LIVERPOOL, LONDON, SWANSEA, MANCHESTER, END}, // EDINBURGH
    {GENEVA, STRASBOURG, ZURICH, MILAN, ROME, BARI, GENOA, NAPLES, END}, // FLORENCE
    {STRASBOURG, ZURICH, PARIS, PRAGUE, LEIPZIG, NUREMBURG, MUNICH, COLOGNE, BERLIN, MILAN, HAMBURG, BRUSSELS, END}, // FRANKFURT
    {BUCHAREST, BUDAPEST, SZEGED, BELGRADE, CONSTANTA, END}, // GALATZ
    {END}, // GALWAY
    {STRASBOURG, ZURICH, MILAN, ROME, GENOA, FLORENCE, END}, // GENEVA
    {GENEVA, STRASBOURG, ZURICH, MILAN, ROME, FLORENCE, END}, // GENOA
    {END}, // GRANADA
    {PRAGUE, LEIPZIG, NUREMBURG, BERLIN, FRANKFURT, VIENNA, END}, // HAMBURG
    {END}, // ST_JOSEPH_AND_ST_MARYS
    {END}, // KLAUSENBURG
    {PARIS, MARSEILLES, COLOGNE, SARAGOSSA, BORDEAUX, BRUSSELS, END}, // LE_HAVRE
    {STRASBOURG, ZURICH, PRAGUE, NUREMBURG, MUNICH, COLOGNE, BERLIN, HAMBURG, BRUSSELS, FRANKFURT, VIENNA, END}, // LEIPZIG
    {MADRID, BARCELONA, SARAGOSSA, ALICANTE, SANTANDER, BORDEAUX, END}, // LISBON
    {EDINBURGH, LONDON, SWANSEA, MANCHESTER, END}, // LIVERPOOL
    {MANCHESTER, LIVERPOOL, SWANSEA, EDINBURGH, END}, // LONDON
    {PARIS, LISBON, BARCELONA, SARAGOSSA, ALICANTE, SANTANDER, BORDEAUX, END}, // MADRID
    {EDINBURGH, LONDON, LIVERPOOL, SWANSEA, END}, // MANCHESTER
    {PARIS, COLOGNE, SARAGOSSA, LE_HAVRE, BORDEAUX, BRUSSELS, END}, // MARSEILLES
    {GENEVA, STRASBOURG, ZURICH, FRANKFURT, ROME, GENOA, FLORENCE, NAPLES, END}, // MILAN
    {NUREMBURG, LEIPZIG, FRANKFURT, BERLIN, END}, // MUNICH
    {END}, // NANTES
    {BARI, MILAN, ROME, FLORENCE, END}, // NAPLES
    {STRASBOURG, PRAGUE, LEIPZIG, MUNICH, COLOGNE, BERLIN, HAMBURG, FRANKFURT, END}, // NUREMBURG
    {MADRID, BARCELONA, MARSEILLES, COLOGNE, SARAGOSSA, LE_HAVRE, BORDEAUX, BRUSSELS, FRANKFURT, END}, // PARIS
    {END}, // PLYMOUTH
    {VENICE, SZEGED, BUDAPEST, LEIPZIG, NUREMBURG, BERLIN, HAMBURG, FRANKFURT, VIENNA, END}, // PRAGUE
    {GENEVA, ZURICH, MILAN, BARI, GENOA, FLORENCE, NAPLES, END}, // ROME
    {VARNA, SOFIA, SZEGED, BELGRADE, END}, // SALONICA
    {MADRID, LISBON, BARCELONA, SARAGOSSA, ALICANTE, BORDEAUX, END}, // SANTANDER
    {PARIS, MADRID, LISBON, BARCELONA, MARSEILLES, ALICANTE, LE_HAVRE, SANTANDER, BORDEAUX, BRUSSELS, END}, // SARAGOSSA
    {END}, // SARAJEVO
    {BUCHAREST, BUDAPEST, BELGRADE, SZEGED, SALONICA, VARNA, END}, // SOFIA
    {GENOA, GENEVA, ZURICH, FLORENCE, LEIPZIG, NUREMBURG, COLOGNE, BERLIN, MILAN, BRUSSELS, FRANKFURT, END}, // STRASBOURG
    {MANCHESTER, LIVERPOOL, LONDON, EDINBURGH, END}, // SWANSEA
    {BUCHAREST, BUDAPEST, BELGRADE, GALATZ, PRAGUE, SOFIA, CONSTANTA, VENICE, SALONICA, VARNA, VIENNA, END}, // SZEGED
    {END}, // TOULOUSE
    {END}, // VALONA
    {SOFIA, SALONICA, SZEGED, BELGRADE, END}, // VARNA
    {BUDAPEST, SZEGED, PRAGUE, BERLIN, VIENNA, END}, // VENICE
    {BUCHAREST, BUDAPEST, BELGRADE, SZEGED, PRAGUE, LEIPZIG, VENICE, BERLIN, HAMBURG, END}, // VIENNA
    {END}, // ZAGREB
    {GENEVA, STRASBOURG, FLORENCE, LEIPZIG, COLOGNE, MILAN, ROME, GENOA, FRANKFURT, END}, // ZURICH
    {END}, // NORTH_SEA
    {END}, // ENGLISH_CHANNEL
    {END}, // IRISH_SEA
    {END}, // ATLANTIC_OCEAN
    {END}, // BAY_OF_BISCAY
    {END}, // MEDITERRANEAN_SEA
    {END}, // TYRRHENIAN_SEA
    {END}, // IONIAN_SEA
    {END}, // ADRIATIC_SEA
    {END} // BLACK_SEA
};

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
    {END}, // ZURICH
    {EDINBURGH, AMSTERDAM, HAMBURG, ATLANTIC_OCEAN, ENGLISH_CHANNEL, END}, // NORTH_SEA
    {PLYMOUTH, LONDON, LE_HAVRE, NORTH_SEA, ATLANTIC_OCEAN, END}, // ENGLISH_CHANNEL
    {DUBLIN, LIVERPOOL, SWANSEA, ATLANTIC_OCEAN, END}, // IRISH_SEA
    {GALWAY, LISBON, CADIZ, NORTH_SEA, IRISH_SEA, ENGLISH_CHANNEL, BAY_OF_BISCAY, MEDITERRANEAN_SEA, END}, // ATLANTIC_OCEAN
    {NANTES, BORDEAUX, SANTANDER, ATLANTIC_OCEAN, END}, // BAY_OF_BISCAY
    {ALICANTE, BARCELONA, MARSEILLES, CAGLIARI, ATLANTIC_OCEAN, TYRRHENIAN_SEA, END}, // MEDITERRANEAN_SEA
    {CAGLIARI, GENOA, ROME, NAPLES, MEDITERRANEAN_SEA, IONIAN_SEA, END}, // TYRRHENIAN_SEA
    {VALONA, ATHENS, SALONICA, TYRRHENIAN_SEA, ADRIATIC_SEA, END}, // IONIAN_SEA
    {VENICE, BARI, IONIAN_SEA, END}, // ADRIATIC_SEA
    {CONSTANTA, VARNA, END}, // BLACK_SEA
};


void generateMessage(HunterView hView, char *message);
LocationID **getDraculaTrails(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail);
void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths);
int validDraculaTrail(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], int *trail);
int validDraculaMove(LocationID from, LocationID to, LocationID histories[NUM_PLAYERS][TRAIL_SIZE]);
LocationID cityID(char name[3]);
int intPow(int base, int index);

int inPath(LocationID *path, LocationID location);

void decideMove(HunterView hView) {
    // backup "default" move for the start
    // rest
    char bestMove[3];
    strcpy(bestMove, names[getLocation(hView, getCurrentPlayer(hView))]);
    
    // BM the crap out of other hunters
    char message[MAX_MESSAGE_SIZE];
    generateMessage(hView, message);
    
    registerBestPlay(bestMove, message);
    
    printf("no segfault yet!\n");
    
    // Initialise all the histories for all the players
    int allHistories[NUM_PLAYERS][TRAIL_SIZE];
    PlayerID currentPlayer;
    for (currentPlayer = 0; currentPlayer < NUM_PLAYERS; currentPlayer ++) {
        getHistory(hView, currentPlayer, allHistories[currentPlayer]);
    }
    
    printf("no segfault yet!\n");
    
    // Begin analysing the information we have, incrementally analysing deeper
    // Get initial trails of length 0 (1 city)
    int numPaths;
    int **draculaTrails = getDraculaTrails(allHistories, NULL, &numPaths, 0);
    int **previousTrails = NULL;
    
    
    printf("no segfault yet!\n");
    
    int depth; // how deep to take the analysis
    for (depth = 1; depth < 6; depth ++) {
        printf("no segfault yet! depth is %d\n", depth);
        previousTrails = draculaTrails;
    
        // Use previous dracula trails to incrementally generate more
        draculaTrails = getDraculaTrails(allHistories, previousTrails, &numPaths, depth);
        
        printf("no segfault yet! depth is %d\n", depth);
        
        // Use all possible dracula trails to evaluate a best move
        getBestMove(hView, bestMove, draculaTrails, numPaths);
        
        printf("best move gotten\n");
        
        free(previousTrails);      
          
        // Finally, register best move and message
        registerBestPlay(bestMove, message);
    }
}

void generateMessage (HunterView hView, char *message) {
    if (getRound(hView) == 0) {
        strcpy(message, "mid or feed");
    } else if (getScore(hView) < 100) {
        strcpy(message, "noob team");
    } else if (getScore(hView) < 200) {
        strcpy(message, "semi-retard team");
    } else {
        strcpy(message, "you guys aren't terrible");
    }
}

LocationID **getDraculaTrails(int histories[NUM_PLAYERS][TRAIL_SIZE], LocationID **previousPaths, int *numPaths, int lengthTrail) {
    // Accepts trails of length n as input, and generates trails of length n + 1 as output
    
    printf("trying to allocate memory...\n");
    
    // Dracula's travel involves a maximum of 8 adjacent cities for every city
    LocationID **generatedTrails = malloc(NUM_MAP_LOCATIONS * intPow(MAX_ADJACENT_LOCATIONS, lengthTrail) * sizeof(int *));
    int numPrevious = *numPaths;
    *numPaths = 0;
    
    printf("%d bytes of memory allocated\n", NUM_MAP_LOCATIONS * intPow(MAX_ADJACENT_LOCATIONS, lengthTrail) * sizeof(int *));
    printf("numprevious is %d\n", numPrevious);
    
    // Generate all the possible trails
    if (lengthTrail == 0) { // previous paths not relevant
        printf("trying to generate initial values\n");
        LocationID currentLocation;
        for (currentLocation = 0; currentLocation < NUM_MAP_LOCATIONS; currentLocation ++) {
            LocationID *initialTrail = malloc(TRAIL_SIZE * sizeof(int));
            initialTrail[0] = currentLocation;
            int i;
            for (i = 1; i < TRAIL_SIZE; i ++) {
                initialTrail[i] = -1;
            }
            if (validDraculaTrail(histories, initialTrail)) {
                generatedTrails[*numPaths] = initialTrail;
                *numPaths = *numPaths + 1;
            }
        }
    } else {
        printf("trying to generate values from previous values!\n");
        int pathIndex;
        for (pathIndex = 0; pathIndex < numPrevious; pathIndex ++) {
            LocationID lastCity = previousPaths[pathIndex][lengthTrail - 1];
            int newIndex;
            for (newIndex = 0; adjacencyRoad[lastCity][newIndex] != END; newIndex ++) {
                printf("newIndex = %d\n", newIndex);
                LocationID *newPath = malloc(TRAIL_SIZE * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = adjacencyRoad[lastCity][newIndex];

                if (validDraculaTrail(histories, newPath)) {
                    generatedTrails[*numPaths] = newPath;
                    *numPaths = *numPaths + 1;
                } else {
                    free(newPath);
                }
            }
            for (newIndex = 0; adjacencySea[lastCity][newIndex] != END; newIndex ++) {
                LocationID *newPath = malloc((lengthTrail + 1) * sizeof(LocationID));
                memcpy(newPath, previousPaths[pathIndex], TRAIL_SIZE * sizeof(LocationID));
                newPath[lengthTrail] = adjacencyRoad[lastCity][newIndex];

                if (validDraculaTrail(histories, newPath)) {
                    generatedTrails[*numPaths] = newPath;
                    *numPaths = *numPaths + 1;
                } else {
                    free(newPath);
                }
            }
        }
    }

    printf("finished generating trails!\n");
    return generatedTrails;
}

int validDraculaTrail(LocationID histories[NUM_PLAYERS][TRAIL_SIZE], int *trail) {
    int i;
    for (i = 0; i < TRAIL_SIZE; i ++) {
        printf("i is %d, trail[i] is %d\n", i, trail[i]);
        // check location is connected to next location
        if (trail[i + 1] != -1) {
            if (!validDraculaMove(trail[i], trail[i + 1], histories)) {
                return FALSE;
            }
        }
        
        // check it matches dracula history
/*        if (histories[PLAYER_DRACULA][i] < NUM_MAP_LOCATIONS && histories[PLAYER_DRACULA][i] != trail[i]) {*/
/*            return FALSE;*/
/*        } else if (histories[PLAYER_DRACULA[i]*/

    }
    
    return TRUE;
}

int validDraculaMove(LocationID from, LocationID to, LocationID histories[NUM_PLAYERS][TRAIL_SIZE]) {
    // Function that checks that dracula could have made that move in that specific turn
    return TRUE; // everything went fine
}

LocationID cityID(char name[3]) {
    // Hash function requires 323 bytes of memory; is injective
    return id[(primes[name[0] - 'A'] * primes[name[1] - 'A'] * primes[name[1] - 'A']) % 323];
}

int intPow(int base, int index) {
    if (index == 0) {
        return 1;
    }
    return base * intPow(base, index - 1);
}

int inPath(LocationID *path, LocationID location) {
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if (path[i] == location) {
            return TRUE;
        }
    }
    return FALSE;
}

// Recursively go through backtrace and create an array of the path
// Returns the length of the path
int rPush(LocationID source, LocationID curLoc, LocationID backtrace[], LocationID **path, int curDistance) {
    if (curLoc == source) {
        *path = malloc(curDistance * sizeof(LocationID));
        (*path)[0] = source;
        return 1;
    }
    int len = rPush(source, backtrace[curLoc], backtrace, path, curDistance + 1) + 1;
    (*path)[len-1] = curLoc;
    
    return len;
}

// Returns distance of path and array containing path by reference
// Returns -1 if no path found
int shortestPath(HunterView hView, LocationID source, LocationID dest, LocationID **path) {
    int found = FALSE;
    int i;

    int seen[NUM_MAP_LOCATIONS];
    LocationID backtrace[NUM_MAP_LOCATIONS];
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        seen[i] = FALSE;
        backtrace[i] = -1;
    }

    Queue q = QueueCreate();
    QueuePush(q, source, source);
    while (!QueueEmpty(q)) {
        queueData data = QueuePop(q);
        if (seen[data.location]) {
            continue;
        }
        seen[data.location] = TRUE;
        backtrace[data.location] = data.from;
        if (data.location == dest) {
            found = TRUE;
            break;
        }

        int numAdjLocs;
        // Remember that Dracula can't travel by rail. As a corollary curRound is irrelevant as well
        LocationID *adjLocs = connectedLocations(hView, &numAdjLocs, data.location, PLAYER_DRACULA,
                                                 0, TRUE, FALSE, TRUE);
        for (i = 0; i < numAdjLocs; i++) {
            if (!seen[adjLocs[i]]) {
                QueuePush(q, adjLocs[i], data.location);
            }
        }
        free(adjLocs);
    }
    QueueDispose(q);
    
    if (found) {
        int temp = rPush(source, dest, backtrace, path, 1);
        return temp;
    } else {
        return -1;
    }
}

void getBestMove(HunterView hView, char *bestMove, LocationID **draculaPaths, int numPaths) {
    // Uses the relevant trail data to predict where dracula could move next, and have the
    // hunter move accordingly

    // First create a TRUE/FALSE array of possible locations Dracula could be in next turn
    LocationID possible[NUM_MAP_LOCATIONS];
    memset(possible, 0, NUM_MAP_LOCATIONS * sizeof(LocationID));

    // Store all the current state information
    PlayerID player = getCurrentPlayer(hView);
    Round curRound = getRound(hView);
    
    printf("Begin filling in the possible locations array...");
    // Begin filling in the possible locations array
    int i;
    for (i = 0; i < numPaths; i++) {
        LocationID curLoc = draculaPaths[i][0];
        int numAdjLocs;
        LocationID *adjLocs = connectedLocations(hView, &numAdjLocs, curLoc, PLAYER_DRACULA,
                                                 curRound, TRUE, FALSE, TRUE);
        
        // For each adjacent location that he could've moved to,
        // increase the corresponding possible array element
        // if it doesn't cross back into his trail
        int j;
        for (j = 0; j < numAdjLocs; j++) {
            // TODO check if dracula has doubled back already
            // and if not, he might be able to double back into his path
            if (!inPath(draculaPaths[i], adjLocs[j])) {
                possible[adjLocs[j]]++;
            }
        }

        // Don't forget to free adjLocs
        free(adjLocs);
    }
    printf("done!\n");
    
    printf("Finding most likely location Dracula is at...");
    // TODO don't use this strat, improve to a better one
    // that takes into account distances etc.
    // Find the most likely location Dracula is at
    LocationID mostLikely = 0;
    int highestProb = 0;
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        if (possible[i] > highestProb) {
            highestProb = possible[i];
            mostLikely = i;
        }
    }
    printf("done! - it's %d\n", mostLikely);
    
    printf("Finding shortest path from %s to %s\n", names[getLocation(hView, player)], names[mostLikely]);
    // Get the first step of the optimal path towards our destination
    LocationID *pathToTake = NULL;
    shortestPath(hView, getLocation(hView, player), mostLikely, &pathToTake);
    printf("Shortest path function called\n");
    LocationID firstStep = getLocation(hView, player);
    if (pathToTake) {
       firstStep = pathToTake[0];
       printf("pathToTake pointing at %p\n", pathToTake);
       free(pathToTake);
    }
    printf("done!\n");

    strcpy(bestMove, names[firstStep]);
}
