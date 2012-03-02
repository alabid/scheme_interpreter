README
======
By Daniel Alabi, Yichen Shen, and Jie Lin.

There are three parts of our scheme intepreter implementation written in C:
1. Tokenizer.
2. Parser.
3. Interpreter.

We closely followed the Scheme standard (which strongly correlates with
the DrRacket implementation). 


What works:
-----------
* primitive list functions: car, cdr, cons (both for proper and
  improper lists).
* primitive arithmetic functions like >=, <, and so on.
* let and its variations like letrec.
* define, set!, let.
* condition operators like  cond, if, or.
* logical operators like and, or.
* lambda 
  
Our interpreter can handle 
i) recursion
ii) currying
iii) functions
iv) Improper lists


We have some complicated test cases that use primitives as a
base and build up to define some recursive functions.




