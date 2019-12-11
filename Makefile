simulate: simulate.o list.o reversi.o logic.o utility.o eval.o book_evaluation.o
	gcc simulate.o list.o reversi.o logic.o utility.o eval.o book_evaluation.o -o simulate -lm -g

simulate.o: simulate.c
	gcc -c simulate.c

list.o: list.c
	gcc -c list.c

reversi.o: reversi.c
	gcc -c reversi.c

logic.o: logic.c
	gcc -c logic.c -lm

utility.o: utility.c
	gcc -c utility.c

eval.o: eval.c
	gcc -c eval.c

book_evaluation.o: book_evaluation.c
	gcc -c book_evaluation.c
