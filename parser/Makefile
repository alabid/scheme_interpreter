main: 	main.o parser.o tokenizer.o
	gcc -g -o parser main.o parser.o tokenizer.o

main.o: main.c
	gcc -c main.c

tokenizer.o: 	tokenizer.c
		gcc -c tokenizer.c

parser.o: 	parser.c
		gcc -c parser.c

clean:
	rm *.o  