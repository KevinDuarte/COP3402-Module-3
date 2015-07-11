#include "scanner.h"
#include "parserwithcodegen.h"
#include "pmachine.h"

int main()
{
  scannermain();
  printf("Scanner successful\n");
  parsermain();
  printf("Parsing and Code Generating successful\n");
  pmachinemain();
  printf("Everything done\n");
  return 0;
}
