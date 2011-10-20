for i in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 15
do
        cat tests/token-test.input.$i | ./tokenizer | diff tests/token-test.output.$i -
done


