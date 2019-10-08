simulate: simulate.o list.o reversi.o logic.o utility.o eval.o
	gcc simulate.o list.o reversi.o logic.o utility.o eval.o -o simulate

simulate.o: simulate.c
	gcc -c simulate.c

list.o: list.c
	gcc -c list.c

reversi.o: reversi.c
	gcc -c reversi.c

logic.o: logic.c
	gcc -c logic.c

utility.o: utility.c
	gcc -c utility.c

eval.o: eval.c
	gcc -c eval.c