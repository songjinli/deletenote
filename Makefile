all: debug

SOURCE=main.c x_lib.c stats.c note.c
DST=mynote

debug:
	gcc -ggdb -o $(DST) $(SOURCE)

release:
	gcc -O2 -o $(DST) $(SOURCE)
	
clean:
	rm -f $(DST)
