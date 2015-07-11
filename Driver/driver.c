#include "scanner.h"
#include "parserwithcodegen.h"
#include "pmachine.h"

int main()
{
  scannermain();
  printf("Scanner successful");
  parsermain();
  printf("Parsing and Code Generating successful");
  pmachinemain();
  printf("Everything done");
  return 0;
}
