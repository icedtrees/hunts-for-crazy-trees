CC=gcc
CFLAGS=-Wall -Werror -g

HUNTER_VIEW_DEPENDENCIES=locations.c locations.h cities.h
HUNTER_VIEW_FILES=HunterView.c locations.c

HUNTER_DEPENDENCIES=$(HUNTER_VIEW_DEPENDENCIES) Queue.c Queue.h hunter.c hunter.h
HUNTER_FILES=$(HUNTER_VIEW_FILES) Queue.c hunter.c


# Compile into object files for the hunter
all: $(HUNTER_DEPENDENCIES)
	$(CC) -c $(HUNTER_FILES) -ljansson


# Tests for the hunter.c
hunterTests: testHunter0 testHunter1 testHunter2 testHunter3

testHunter0: $(HUNTER_DEPENDENCIES) testHunter0.c
	$(CC) $(CFLAGS) -o testHunter0 $(HUNTER_FILES) testHunter0.c

testHunter1: $(HUNTER_DEPENDENCIES) testHunter1.c
	$(CC) $(CFLAGS) -o testHunter1 $(HUNTER_FILES) testHunter1.c

testHunter2: $(HUNTER_DEPENDENCIES) testHunter2.c
	$(CC) $(CFLAGS) -o testHunter2 $(HUNTER_FILES) testHunter2.c

testHunter3: $(HUNTER_DEPENDENCIES) testHunter3.c
	$(CC) $(CFLAGS) -o testHunter3 $(HUNTER_FILES) testHunter3.c


# Tests for the HunterView.c
hunterViewTests: testHunterView testHunterView1 testHunterView2 testHunterView3 testHunterView4

testHunterView: $(HUNTER_VIEW_DEPENDENCIES) testHunterView.c
	$(CC) $(CFLAGS) -o testHunterView0 $(HUNTER_VIEW_FILES) testHunterView.c

testHunterView1: $(HUNTER_VIEW_DEPENDENCIES) testHunterView1.c
	$(CC) $(CFLAGS) -o testHunterView1 $(HUNTER_VIEW_FILES) testHunterView1.c

testHunterView2: $(HUNTER_VIEW_DEPENDENCIES) testHunterView2.c
	$(CC) $(CFLAGS) -o testHunterView2 $(HUNTER_VIEW_FILES) testHunterView2.c

testHunterView3: $(HUNTER_VIEW_DEPENDENCIES) testHunterView3.c
	$(CC) $(CFLAGS) -o testHunterView3 $(HUNTER_VIEW_FILES) testHunterView3.c

testHunterView4: $(HUNTER_VIEW_DEPENDENCIES) testHunterView4.c
	$(CC) $(CFLAGS) -o testHunterView4 $(HUNTER_VIEW_FILES) testHunterView4.c


# Remove all the temporary files
clean :
	rm -rf myPlayer testHunterView? testHunter? *.o core vgcore.* *.dSYM debug.txt
