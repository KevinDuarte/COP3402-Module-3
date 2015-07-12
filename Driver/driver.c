#include "scanner.h"
#include "parserwithcodegen.h"
#include "pmachine.h"
#include "string.h"

int main(int argc, char** argv)
{
  int i;
  int lFlag;
  int aFlag;
  int vFlag;

  //Loops through arguments, argv[0] is file name, file arguments
  //start at argv[1], will loop through argv[1] to argv[argc-1]
  //and set flags based on commands, if the directives are incorrectly formatted
  //they are ignored
  for(i = 1; i < argc; i++)
  {
    if(strcmp(argv[i],"-l") == 0)
    {
      lFlag = 1;
    }
    else if(strcmp(argv[i], "-a") == 0)
    {
      aFlag = 1;
    }
    else if(strcmp(argv[i], "-v") == 0)
    {
      vFlag = 1;
    }
  }
  char buffer[3][10]
  strcpy(buffer[1],"print");

  if(lFlag == 1)
  {
    scannermain(2,buffer)
  }
  else
  {
    scannermain(0, buffer);
  }
  printf("Program was successfully scanned in.\n");

  if(aFlag == 1)
  {
    parsermain(2,buffer);
  }
  else
  {
    parsermain(0,buffer);
  }
  printf("No errors, program is syntactically correct.\n");

  if(vFlag == 1)
  {
    pmachine(2, buffer);
  }
  else
  {
    pmachinemain(0,buffer);
  }
  printf("Stacktrace successfully created\n");
  return 0;
}
