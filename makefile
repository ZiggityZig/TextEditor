all:textEditor

textEditor:  main.o
	gcc -m32 -g -Wall -o textEditor main.o

main.o: main.c
	gcc -m32 -g -Wall -c -o main.o main.c

.PHONY: clean

clean: 
	rm -f *.o textEditor
