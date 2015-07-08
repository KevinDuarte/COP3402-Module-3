#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol
{
int kind; // const = 1, var = 2, proc = 3
char name[12]; // name up to 11 chars
int val; // number (ASCII value)
int level; // L level
int addr; // M address
} symbol;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

//the lexemelist fp
FILE * ifp;
int TOKEN;
char IDENTIFIER[12];
int NUMBER;
int numOfSymbols = 0;
int numOfVariables = 0;

//number symbol values
typedef enum
{
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33
}token_type;


//returns that an error has occured
void ERROR(char error[])
{
    printf("%s\n", error);
    exit(1);
}
//gets the next token GET(TOKEN) in the psuedo
void GETTOKEN()
{
    //scans in the next token, if there is no token, it is given the value -1
    if(fscanf(ifp, "%d", &TOKEN) == EOF)
    {
        TOKEN = -1;
    }
    //gets the identifier
    if(TOKEN == identsym)
    {
        fscanf(ifp, "%s", IDENTIFIER);
    }
    //gets the number
    if(TOKEN == numbersym)
    {
        fscanf(ifp, "%d", &NUMBER);
    }
}
//enters a new symbol into the symbol table
// kind: const = 1, var = 2, proc = 3
void ENTER(int kind)
{
    //ensures a symbol is in the symbol table once
    int i;
    for(i = 0; i < numOfSymbols; i++)
    {
        if(strcmp(symbol_table[i].name, IDENTIFIER) == 0)
        {
            ERROR("Error number 27, an identifier has been declared multiple times.");
        }
    }

    symbol_table[numOfSymbols].kind = kind;
    strcpy(symbol_table[numOfSymbols].name, IDENTIFIER);

    if(kind == 1)
    {
        symbol_table[numOfSymbols].val = NUMBER;
        //level is 0 since this is tiny PL/0
        symbol_table[numOfSymbols].level = 0;
        symbol_table[numOfSymbols].addr = 0;
    }
    if(kind == 2)
    {
        //level is 0 since this is tiny PL/0
        symbol_table[numOfSymbols].level = 0;
//TODO find what the base pointer is (0 or 1)
        int basePointer = 0;
        symbol_table[numOfSymbols].addr = basePointer + 4 + numOfVariables;
        numOfVariables++;
    }

    numOfSymbols++;
}


//checks if the token is a relational operator ( = | <> | < | <= | > | >= )
int isRelationalOperator(int token)
{
    return(token == eqlsym || token == neqsym || token == lessym || token == leqsym || token == gtrsym || token == geqsym);
}
//returns the symbol/indentifier with the given name
symbol getSymbol()
{
    int i;
    for(i = 0; i < numOfVariables; i++)
    {
        if(strcmp(symbol_table[i].name, IDENTIFIER))
            return symbol_table[i];
    }
}


void FACTOR();
void TERM();
void EXPRESSION();
void CONDITION();
void STATEMENT();
void BLOCK();
void PROGRAM();


void FACTOR()
{
    if(TOKEN == identsym)
    {
/*
if getSymbol() == variable
    LOD getSymbol().L getSymbol().M
    possible ERROR(variable has not been initialized)
else if getSymbol() == constant
    INC 0 1
    LIT 0 getSymbol().val
*/

        GETTOKEN();
    }
    else if(TOKEN == numbersym)
    {
/*
INC 0 1
LIT 0 NUMBER
*/

        GETTOKEN();
    }
    else if(TOKEN == lparentsym)
    {
        GETTOKEN();
        EXPRESSION();
        if(TOKEN != rparentsym)
        {
            ERROR("Error number 22, right parenthesis missing.");
        }
        GETTOKEN();
    }
    else
    {
        printf("%d\n", TOKEN);
        ERROR("Error number 23, the preceding factor cannot begin with this symbol.");
    }
}

void TERM()
{
    FACTOR();
    while(TOKEN == multsym || TOKEN == slashsym)
    {
        GETTOKEN();
        FACTOR();
/*
OPR 0 4 (for mult)
OPR 0 5 (for div)
*/
    }
}

void EXPRESSION()
{
    if(TOKEN == plussym || TOKEN == minussym)
    {
/*
if(minussym)

OPR 0 1
*/
        GETTOKEN();
    }
    TERM();
    while(TOKEN == plussym || TOKEN == minussym)
    {
        GETTOKEN();
        TERM();
/*
OPR 0 2 (for add)
OPR 0 3 (for sub)
*/
    }
}

void CONDITION()
{
    if(TOKEN == oddsym)
    {
        GETTOKEN();
        EXPRESSION();
/*
OPR 0 6
*/
    }
    else
    {
        EXPRESSION();
        if(!isRelationalOperator(TOKEN))
        {
            ERROR("Error number 20, relational operator expected.");
        }
        GETTOKEN();
        EXPRESSION();

/*
OPR 0 [conditionValue]
*/
    }
}

void STATEMENT()
{
    //initialization
    if(TOKEN == identsym)
    {
        char name[12];
        strcpy(name, IDENTIFIER);

        //looks for the identifier in the symbol table
        int i, found = 0;
        for(i = 0; i < numOfSymbols; i++)
        {
            if(strcmp(symbol_table[i].name, IDENTIFIER) == 0)
            {
                found = 1;
                //if identifier is not a variable, produce error
                if(symbol_table[i].kind != 2)
                {
                    ERROR("Error number 12, assignment to constant or procedure not allowed.");
                }
            }
        }
        if(found == 0)
        {
            ERROR("Error number 11, undeclared identifier.");
        }

        GETTOKEN();
        if(TOKEN != becomessym)
        {
            ERROR("Error number 13, assignment operator expected.");
        }
        GETTOKEN();
        EXPRESSION();
/*
STO n.L n.M
*/
    }
    //procedure call (not in tiny PL/0)
    else if(TOKEN == callsym)
    {
        GETTOKEN();
        if(TOKEN != identsym)
        {
            ERROR("Error number 14, call must be followed by an identifier.");
        }

        //looks for the identifier in the symbol table
        int i, found = 0;
        for(i = 0; i < numOfSymbols; i++)
        {
            if(strcmp(symbol_table[i].name, IDENTIFIER) == 0)
            {
                found = 1;
                //if the identifier is not a procedure, produce an error
                if(symbol_table[i].kind != 3)
                {
                    ERROR("Error number 15, call of a constant or variable is meaningless.");
                }
            }
        }
        if(found == 0)
        {
            ERROR("Error number 11, undeclared identifier.");
        }

        GETTOKEN();
    }
    //a group of statements
    else if(TOKEN == beginsym)
    {
        GETTOKEN();
        STATEMENT();
        while(TOKEN == semicolonsym)
        {
            GETTOKEN();
            STATEMENT();
        }
        if(TOKEN != endsym)
        {
            ERROR("Error number 26, end is expected.");
        }
        GETTOKEN();
    }
    else if(TOKEN == ifsym)
    {
        GETTOKEN();
        CONDITION();
/*top of the stack has whether it is true or false
  make branch here
*/

        if(TOKEN != thensym)
        {
            ERROR("Error number 16, then expected.");
        }
        GETTOKEN();
        STATEMENT();
    }
    else if(TOKEN == whilesym)
    {
        GETTOKEN();
        CONDITION();
/*top of the stack has whether it is true or false
  make branch here
*/
        if(TOKEN != dosym)
        {
            ERROR("Error number 18, do expected.");
        }
        GETTOKEN();
        STATEMENT();
/*jump back to branch
*/
    }
}

void BLOCK()
{
    if(TOKEN == constsym)
    {
        do
        {
            GETTOKEN();
            if(TOKEN != identsym)
            {
                ERROR("Error number 4, const must be followed by identifier.");
            }
            GETTOKEN();
            if(TOKEN == becomessym)
            {
                ERROR("Error number 1, use = instead of :=");
            }
            if(TOKEN != eqlsym)
            {
                ERROR("Error number 2, identifier must be followed by =");
            }
            GETTOKEN();
            if(TOKEN != numbersym)
            {
                ERROR("Error number 3, = must be followed by a number.");
            }
            //the 0 is unnecissary
            ENTER(1);
            GETTOKEN();
        } while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            ERROR("Error number 5, semicolon or comma missing.");
        }
        GETTOKEN();
    }
    if(TOKEN == varsym)
    {
        do
        {
            GETTOKEN();
            if(TOKEN != identsym)
            {
                ERROR("Error number 4, var must be followed by identifier.");
            }
            //what level do i input?
            ENTER(2);
            GETTOKEN();
        } while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            ERROR("Error number 5, semicolon or comma missing.");
        }
        GETTOKEN();
    }
    //procedure section, will not be used in module 3
    while(TOKEN == procsym)
    {
        GETTOKEN();
        if(TOKEN != identsym)
        {
            ERROR("Error number 4, procedure must be followed by identifier.");
        }
        //The 0 is useless here
        ENTER(3);
        GETTOKEN();
        if(TOKEN != semicolonsym)
        {
            ERROR("Error number 6, incorrect symbol after procedure declaration.");
        }
        GETTOKEN();
        BLOCK();
        if(TOKEN != semicolonsym)
        {
            //may need to be a different error message
            ERROR("Error number 5, semicolon or comma missing.");
        }
        GETTOKEN();
    }
//inc stack

/*
INC 0 (4 + numOfVariables)
*/

    STATEMENT();
}

void PROGRAM()
{
    GETTOKEN();
    BLOCK();
    if(TOKEN != periodsym)
    {
        ERROR("Error number 9, period expected.");
    }
}


int main()
{
    ifp = fopen("lexemelist.txt", "r");
    PROGRAM();
    printf("parsing finished\n");
    int i;
    for(i = 0; i < numOfSymbols; i++)
    {
        printf("%d %s\n", symbol_table[i].kind, symbol_table[i].name);
    }
    fclose(ifp);
}
