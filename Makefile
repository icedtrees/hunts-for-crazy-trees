CC=gcc
CFLAGS=-Wall -Werror -g

all:
	gcc -c hunter.c HunterView.c Queue.c -ljansson

tests: testHunterView testHunterView1 testHunterView2 testHunterView3 testHunterView4

testHunterView : testHunterView.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView testHunterView.c HunterView.c

testHunterView1 : testHunterView1.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView1 testHunterView1.c HunterView.c

testHunterView2 : testHunterView2.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView2 testHunterView2.c HunterView.c

testHunterView3 : testHunterView3.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView3 testHunterView3.c HunterView.c

testHunterView4 : testHunterView4.c HunterView.c
	$(CC) $(CFLAGS) -o testHunterView4 testHunterView4.c HunterView.c

clean :
	rm -rf myPlayer testHunterView testHunterView? *.o core *.dSYM
