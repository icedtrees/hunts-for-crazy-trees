CC=gcc
CFLAGS=-Wall -Werror -g

all: testHunterView testHunterView1 testHunterView2 testHunterView3

testHunterView : testHunterView.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView testHunterView.c HunterView.c

testHunterView1 : testHunterView1.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView1 testHunterView1.c HunterView.c

testHunterView2 : testHunterView2.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView2 testHunterView2.c HunterView.c

testHunterView3 : testHunterView3.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView3 testHunterView3.c HunterView.c

clean :
	rm -f testHunterView? *.o core *.dSYM
