all : client.out serveur.out

common.o : common.h common.c
	gcc -c -o common.o common.c

client.out : client.c common.o
	gcc -o client.out client.c common.o

serveur.out : serveur.c common.o
	gcc -o serveur.out serveur.c common.o

clean :
	rm -f *.o && rm -f *.out && clear
