main:	main.o tokenizer.o
	gcc -g -o tokenizer main.o tokenizer.o

main.o:	main.c
	gcc -c main.c

tokenizer.o:	tokenizer.c
		gcc -c tokenizer.c

clean:
	rm *.o 

