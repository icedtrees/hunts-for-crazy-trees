all:
	gcc -c hunter.c HunterView.c -ljansson

clean:
	rm -rf myPlayer
