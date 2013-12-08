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
  
Our interpreter can handle:

* recursion
* currying
* functions
* Improper lists


We have some complicated test cases that use primitives as a
base and build up to define some recursive functions.


MIT Open Source License
-----------------------

Copyright © 2012 Daniel Alabi

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

