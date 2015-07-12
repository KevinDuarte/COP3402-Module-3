#include "scanner.h"
#include "parserwithcodegen.h"
#include "pmachine.h"
#include <stdio.h>
#include <string.h>

void printLexemelist()
{
    FILE *ifp = fopen("lexemelist.txt", "r");

    char buffer;

    while((buffer = fgetc(ifp)) != EOF)
    {
        printf("%c", buffer);
    }
    printf("\n\n");

    fclose(ifp);
}

void printAssembly()
{
    FILE *ifp = fopen("mcode.txt", "r");

    char buffer;

    while((buffer = fgetc(ifp)) != EOF)
    {
        printf("%c", buffer);
    }
    printf("\n");

    fclose(ifp);
}

void printStackTrace()
{
    FILE *ifp = fopen("stacktrace.txt", "r");

    char buffer;

    while((buffer = fgetc(ifp)) != EOF)
    {
        printf("%c", buffer);
    }
    printf("\n");

    fclose(ifp);
}



int main(int argc, char** argv)
{
    int lFlag = 0;
    int aFlag = 0;
    int vFlag = 0;

    int i;
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

    scannermain();
    printf("Program was successfully scanned in.\n\n");
    if(lFlag == 1)
    {
        printLexemelist();
    }

    parsermain();
    printf("No errors, program is syntactically correct.\n\n");
    if(aFlag == 1)
    {
        printAssembly();
    }

    pmachinemain();
    printf("Stacktrace successfully created.\n\n");
    if(vFlag == 1)
    {
        printStackTrace();
    }

    return 0;
}
