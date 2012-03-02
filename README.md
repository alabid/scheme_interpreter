README
======
By Daniel Alabi, Yichen Shen, and Jie Lin.

There are three parts of our scheme intepreter implementation written in C:
* Tokenizer.
* Parser.
* Interpreter.

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
+ recursion
+ currying
+ functions
+ Improper lists


We have some complicated test cases that use primitives as a
base and build up to define some recursive functions.




