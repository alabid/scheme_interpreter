main: 	main.o interpreter.o parser.o tokenizer.o value.o
	gcc -g -o interpreter main.o interpreter.o parser.o tokenizer.o value.o

main.o: main.c
	gcc -c main.c

value.o: 	value.c
		gcc -c value.c

tokenizer.o: 	tokenizer.c
		gcc -c tokenizer.c

parser.o: 	parser.c
		gcc -c parser.c

interpreter.o: 	interpreter.c
		gcc -c interpreter.c

clean:
	rm *.o  