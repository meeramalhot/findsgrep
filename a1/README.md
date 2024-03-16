finds program
support for parenthesis

call using:
./finds -p pathname [-f c|h|S] [-l] -s s

example:

./finds -p /home/ugrad/meeram/a1/testdir -s "a.b"


printf program
can only print with same type of argument cannot do mixed arguments
it can take multiple of the same argument
only works with 64 bit

testcases within printf
examples:
  my_printf("%u", 39);
  my_printf("%x", 39);
  my_printf("%d",10);
  my_printf("%s", "hi");
  my_printf("%d %d %d %d %d %d %d %d %d %d", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
  my_printf("%s, %s", "hello", "How are you");
  
execute with:
./my_printf
