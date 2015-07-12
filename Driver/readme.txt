//TEAM
//First team member name: Kevin Duarte
//Second team member name: Nestor Maysonet

//How to compile:

  gcc driver.c parserwithcodegen.c scanner.c pmachine.c -o driver

//How to run (ensure there is an input.txt with the PL/0 input code):

  ./driver

  //after ./driver you may use the comiler directives -l -a -v to print to the console


//The compiler will create these .txt files:
  //cleaninput.txt: the input.txt file without comments
  //lexemetable.txt: a table with all the lexemes/tokens
  //lexemelist.txt: a list of all the lexemes/tokens
  //mcode.txt: the assembly code that is generated
  //stacktrace.txt: the stacktrace that is generated from the assembly code

