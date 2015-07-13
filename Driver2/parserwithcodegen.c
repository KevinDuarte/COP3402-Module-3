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
//output file pointer
FILE* ofp;
//Most recent token
int TOKEN;
//most recent identifier or number
char IDENTIFIER[12];
int NUMBER;

//number of symbols in symbol_table
int numOfSymbols = 0;
//number of variables in symbol_table
int numOfVariables = 0;

//used for the branching in conditionals
int inConditional = 0;

//counts the amount of lines of assembly that have been written
int lines = 0;

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
        //Base pointer starts at 1
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

//returns the symbol/identifier with the given name
symbol getSymbol(char identifier[])
{
    int i, found = 0;
    for(i = 0; i < numOfSymbols; i++)
    {
        if(strcmp(symbol_table[i].name, identifier) == 0)
            return symbol_table[i];
    }
    if(found == 0)
        ERROR("Error number 11, undeclared identifier.");
}

//used to print the assembly code to the output file
void printToFile(int op, int L, int M)
{
    //if you are in a conditional, it does not print
    if(inConditional != 0)
        return;

    //print to the file here
    fprintf(ofp,"%d %d %d \n", op, L, M);
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
        symbol current = getSymbol(IDENTIFIER);
        if(current.kind == 2)
        {
            //LOD current.L current.M
            printToFile(3,0,current.addr);
            lines++;
        }
        else if(current.kind == 1)
        {
            //LIT 0 getSymbol().val
            printToFile(1,0,current.val);
            lines++;
        }
        else
        {
            ERROR("Error 21: expression must not contain a procedure identifier.");
        }

        GETTOKEN();
    }
    else if(TOKEN == numbersym)
    {
        //LIT 0 NUMBER
        printToFile(1,0,NUMBER);
        lines++;

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
        if(TOKEN == multsym)
        {
           GETTOKEN();
           FACTOR();
           //OPR 0 4
           printToFile(2,0,4);
           lines++;
        }
        else if(TOKEN == slashsym)
        {
            GETTOKEN();
            FACTOR();
            //OPR 0 5
            printToFile(2,0,5);
            lines++;
        }
    }
}

void EXPRESSION()
{
    if(TOKEN == plussym || TOKEN == minussym)
    {
        if(TOKEN == minussym)
        {
            GETTOKEN();
            //OPR 0 1
            printToFile(2,0,1);
            lines++;
        }
        else
        {
            GETTOKEN();
        }
    }
    TERM();
    while(TOKEN == plussym || TOKEN == minussym)
    {
        if(TOKEN == plussym)
        {
            GETTOKEN();
            TERM();
            printToFile(2,0,2);
            lines++;
        }
        else if(TOKEN == minussym)
        {
            GETTOKEN();
            TERM();
            printToFile(2,0,3);
            lines++;
        }
    }
}

void CONDITION()
{
    if(TOKEN == oddsym)
    {
        GETTOKEN();
        EXPRESSION();
        //OPR 0 6
        printToFile(2,0,6);
        lines++;
    }
    else
    {
        EXPRESSION();
        if(!isRelationalOperator(TOKEN))
        {
            ERROR("Error number 20, relational operator expected.");
        }
        //In the enum, the tokens for logical operators are only +1
        //from their corresponding OPR M value, so subtracting by one
        //should get the correct M value.
        int conditionValue = TOKEN-1;
        GETTOKEN();
        EXPRESSION();

        //OPR 0 M
        printToFile(2,0,conditionValue);
        lines++;
    }
}

void STATEMENT()
{
    //initialization
    if(TOKEN == identsym)
    {
        //stores the name of the identifier that will be initialized
        char name[12];
        strcpy(name, IDENTIFIER);

        //if identifier is not a variable, produce error
        if(getSymbol(IDENTIFIER).kind != 2)
        {
            ERROR("Error number 12, assignment to constant or procedure not allowed.");
        }

        GETTOKEN();
        if(TOKEN != becomessym)
        {
            ERROR("Error number 13, assignment operator expected.");
        }
        GETTOKEN();
        EXPRESSION();

        symbol current = getSymbol(name);
        //STO 0 M
        printToFile(4,current.level,current.addr);
        lines++;
    }
    //procedure call (not in tiny PL/0)
    else if(TOKEN == callsym)
    {
        GETTOKEN();
        if(TOKEN != identsym)
        {
            ERROR("Error number 14, call must be followed by an identifier.");
        }

        //if the identifier is not a procedure, produce an error
        if(getSymbol(IDENTIFIER).kind != 3)
        {
            ERROR("Error number 15, call of a constant or variable is meaningless.");
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
//top of the stack has whether it is true or false
        if(TOKEN != thensym)
        {
            ERROR("Error number 16, then expected.");
        }

        //after the condition, count how many instructions are written
        int currentLines = lines;
        inConditional++;
        fpos_t filePos;
        fgetpos(ifp, &filePos);

        //loop ensures this is done twice
        int i;
        for(i = 0; i < 2; i++)
        {
            if(i == 1)
            {
                inConditional--;
//make branch here (lines contains the line that you jump to if the condition is not met)
//printToFile()
                //JPC 0 M = lines
                printToFile(8,0,lines);

                //returns the file to the previous position
                fsetpos(ifp, &filePos);
                lines = currentLines;
                //Lines increment for prinToFile used in for loop
                //lines++;
            }
            lines++;

            GETTOKEN();
            STATEMENT();
        }

    }
    else if(TOKEN == whilesym)
    {
        int jumpBackLine = lines;
        GETTOKEN();
        CONDITION();
//top of the stack has whether it is true or false
        if(TOKEN != dosym)
        {
            ERROR("Error number 18, do expected.");
        }

        //after the condition, count how many instructions are written
        int currentLines = lines;
        inConditional++;
        fpos_t filePos;
        fgetpos(ifp, &filePos);

        //loop ensures this is done twice
        int i;
        for(i = 0; i < 2; i++)
        {
            if(i == 1)
            {
                inConditional--;
//make branch here (lines + 1 contains the line that you jump to if the condition is not met)
//printToFile()
                //JPC 0 M = l
                printToFile(8,0,lines + 1);

                //returns the file to the previous position
                fsetpos(ifp, &filePos);
                lines = currentLines;
                //Lines increment for the printToFile used in for loop
                //lines++;
            }
            //the line for the branch is added
            lines++;

            GETTOKEN();
            STATEMENT();
        }
        //JMP 0 M = jumpBackLines
        printToFile(7,0,jumpBackLine);
        lines++;
    }
    else if(TOKEN == readsym)
    {
        GETTOKEN();
        if(TOKEN != identsym)
        {
            ERROR("Error number 28, identifier expected after read.");
        }

        symbol current = getSymbol(IDENTIFIER);

        if(current.kind != 2)
        {
            ERROR("Error number 29, writing to a constant or procedure is not allowed.");
        }

        //SIO 0 1
        //STO 0 M
        printToFile(9, 0, 1);
        printToFile(4,current.level,current.addr);
        lines += 2;

        GETTOKEN();
    }
    else if(TOKEN == writesym)
    {
        GETTOKEN();
        if(TOKEN != identsym)
        {
            ERROR("Error number 30, identifier expected after write.");
        }

        symbol current = getSymbol(IDENTIFIER);

        if(current.kind == 3)
        {
            ERROR("Error number 31, cannot write a procedure.");
        }

        if(current.kind == 2)
        {
            //LOD L M
            printToFile(3, current.level, current.addr);
        }
        if(current.kind == 1)
        {
            //LIT 0 val
            printToFile(1, 0, current.val);
        }
        //SIO 0 0
        printToFile(9, 0, 0);
        lines +=2;

        GETTOKEN();
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
            ENTER(2);
            GETTOKEN();
        } while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            ERROR("Error number 5, semicolon or comma missing.");
        }
        GETTOKEN();
    }
    //procedure section (will not be used in module 3)
    while(TOKEN == procsym)
    {
        GETTOKEN();
        if(TOKEN != identsym)
        {
            ERROR("Error number 4, procedure must be followed by identifier.");
        }
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
    //INC O (4+numOfVariables)
    printToFile(6,0,4+numOfVariables);
    lines++;

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
    //the halt at the end of the program
    printToFile(9, 0, 2);
}


int parsermain()
{
    ifp = fopen("lexemelist.txt", "r");
    ofp = fopen("mcode.txt","w");

    PROGRAM();
    fclose(ifp);
    fclose(ofp);
}
