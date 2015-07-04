#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
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

//number symbol values
typedef enum
{
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33
}token_type;

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
//returns that an error has occured
void ERROR(char error[])
{
    printf("%s", error);
    exit(1);
}
//checks if the token is a relational operator ( = | <> | < | <= | > | >= )
int isRelationalOperator(int token)
{
    return(token == eqlsym || token == neqsym || token == lessym || token == leqsym || token == gtrsym || token == geqsym);
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
        GETTOKEN();
    }
    else if(TOKEN == NUMBER)
    {
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
    }
}

void EXPRESSION()
{
    if(TOKEN == plussym || TOKEN == minussym)
    {
        GETTOKEN();
    }
    TERM();
    while(TOKEN == plussym || TOKEN == minussym)
    {
        GETTOKEN();
        TERM();
    }
}

void CONDITION()
{
    if(TOKEN == oddsym)
    {
        GETTOKEN();
        EXPRESSION();
    }
    else
    {
        EXPRESSION();
        //TODO make an isRelation function
        if(!isRelationalOperator(TOKEN))
        {
            ERROR("Error number 20, relational operator expected.");
        }
        GETTOKEN();
        EXPRESSION();
    }
}

void STATEMENT()
{
    if(TOKEN == identsym)
    {
        GETTOKEN();
        if(TOKEN != becomessym)
        {
            ERROR("Error number 13, assignment operator expected.");
        }
        GETTOKEN();
        EXPRESSION();
    }
    else if(TOKEN == callsym)
    {
        GETTOKEN();
        if(TOKEN != identsym)
        {
            ERROR("Error number 14, call must be followed by an identifier.");
        }
        GETTOKEN();
    }
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
        if(TOKEN != thensym)
        {
            ERROR("Error number 16, then expected.");
        }
        GETTOKEN();
        STATEMENT();
    }
    else if(TOKEN = whilesym)
    {
        GETTOKEN();
        CONDITION();
        if(TOKEN != dosym)
        {
            ERROR("Error number 18, do expected.");
        }
        GETTOKEN();
        STATEMENT();
    }
    else
    {
        ERROR("Error number 7, statement expected.")
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
            if(TOKEN != eqlsym)
            {
                ERROR("Error number 2, identifier must be followed by =");
            }
            GETTOKEN();
            if(TOKEN != numbersym)
            {
                ERROR("Error number 3, = must be followed by a number.");
            }
            GETTOKEN();
        } while(TOKEN != commasym);

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
            GETTOKEN();
        } while(TOKEN != commasym);

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
        GETTOKEN();
        if(TOKEN != semicolonsym)
        {
            ERROR("Error number 6, incorrect symbol after procedure declaration.");
        }
        GET(TOKEN);
        BLOCK();
        if(TOKEN != semicolonsym)
        {
            ERROR("Error number 5, semicolon or comma missing.");
        }
        GETTOKEN();
    }
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
    fclose(ifp);
}
