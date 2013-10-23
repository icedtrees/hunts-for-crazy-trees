#include "locations.h"

LocationID cityID(char name[3]) {
    // Hash function requires 323 bytes of memory; no collisions
    return id[(primes[name[0] - 'A'] * primes[name[1] - 'A'] * primes[name[1] - 'A']) % 323];
}