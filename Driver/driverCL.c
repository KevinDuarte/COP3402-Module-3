#include "scanner.h"
#include "parserwithcodegen.h"
#include "pmachine.h"
#include String.h

int main(int argc, char** argv)
{
  int flagL = 0; //Lexeme list
  int flagV = 0; //Virtual Machine
  int flagA = 0; //Assembly
  
  int i = 0;
  for(i = 1; i < argc; i++)
  {
    if(!strcmp(argv[i],"-l")
    {
      flagL = 1;
    }
    else if(!strcmp(argv[i], "-v"))
    {
      flagV = 1;
    }
    else if(!strcmp(argv[i], "-a"))
    {
      flagA = 1;
    }
  }
  
  //Lexer
  if( flagL == 1)
  { 
    scannermain("print");
    printf("Scanner successful\n");
  }
  else if( flagL == 0)
  {
    scannermain();
    printf("Scanner successful\n");
  }
  
  //Parser/Code
  if(flagA == 1)
  {
    parsermain("print");
    printf("Parsing and Code Generating successful\n");
  }
  else if(flagA == 0)
  {
    parsermain();
    printf("Parsing and Code Generating successful\n");
  }
  
  //Virtual Machine
  if(flagV == 1)
  {
    pmachinemain("print");
    printf("Everything done\n");
  }
  else if(flagV == 0)
  {
    pmachinemain();
    printf("Everything done\n");
  }
  return 0;
}
