all : main1 main2 main3 main5 myGit

main1.o : main1.c header/Listes.h
	gcc -g -o main1.o -c main1.c

main2.o : main2.c header/Listes.h header/Works.h
	gcc -g -o main2.o -c main2.c

main3.o : main3.c header/Listes.h header/Works.h header/Commit.h
	gcc -g -o main3.o -c main3.c

main5.o : main5.c header/Listes.h header/Works.h header/Commit.h header/Branch.h header/Merge.h
	gcc -g -o main5.o -c main5.c

myGit.o : myGit.c header/Listes.h header/Works.h header/Commit.h header/Branch.h header/Merge.h
	gcc -g -o myGit.o -c myGit.c

semaine1.o : semaine1.c header/Listes.h 
	gcc -g -o semaine1.o -c semaine1.c

semaine2.o : semaine2.c header/Listes.h header/Works.h 
	gcc -g -o semaine2.o -c semaine2.c

semaine3.o : semaine3.c header/Listes.h header/Works.h header/Commit.h
	gcc -g -o semaine3.o -c semaine3.c

semaine4.o : semaine4.c header/Listes.h header/Works.h header/Commit.h header/Branch.h
	gcc -g -o semaine4.o -c semaine4.c

semaine5.o : semaine5.c header/Listes.h header/Works.h header/Commit.h header/Branch.h header/Merge.h
	gcc -g -o semaine5.o -c semaine5.c

main1 : main1.o semaine1.o
	gcc -o main1 main1.o semaine1.o 

main2 : main2.o semaine1.o semaine2.o
	gcc -o main2 main2.o semaine1.o semaine2.o

main3 : main3.o semaine1.o semaine2.o semaine3.o
	gcc -o main3 main3.o semaine1.o semaine2.o semaine3.o

main5 : main5.o semaine1.o semaine2.o semaine3.o semaine4.o semaine5.o
	gcc -o main5 main5.o semaine1.o semaine2.o semaine3.o semaine4.o semaine5.o

myGit : myGit.o semaine1.o semaine2.o semaine3.o semaine4.o semaine5.o
	gcc -o myGit myGit.o semaine1.o semaine2.o semaine3.o semaine4.o semaine5.o

clean :
	rm -f *.o main
