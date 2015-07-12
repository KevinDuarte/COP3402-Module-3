#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define cmax 11

//Lexical Analyser code here

//token struct
typedef struct
{
    int class;
    char lexeme[cmax];
}token_t;

//number symbol values, also used for state values
typedef enum
{
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33
}token_type;

//reserved words
char *word[ ] =
{
"null", "begin", "call", "const", "do", "else", "end", "if",
"odd", "procedure", "read", "then", "var", "while", "write"
};
//corresponding reserved word values
int wsym [ ] =
{
nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym,
ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym
};

//special symbols
//editted for the semicolon at the end
char symbols[] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};


//returns 1 if a character is a letter, 0 otherwise
int isALetter(char ch)
{
    //checks lowercase
    int value = ch - 'a';
    if(value >= 0 && value < 26)
        return 1;

    //checks uppercase
    int value2 = ch - 'A';
    if(value2 >= 0 && value2 < 26)
        return 1;

    return 0;
}
//returns 1 if a character is a digit, 0 otherwise
int isADigit(char ch)
{
    int value = ch - '0';
    if(value >=0 && value < 10)
        return 1;
    return 0;
}
//returns 1 if a character is a special symbol, 0 otherwise
int isASpecialSymbol(char ch)
{
    int i;
    for(i=0; i < 13; i++)
    {
        if(ch == symbols[i])
            return 1;
    }
    return 0;
}
//returns 1 if a character is white space, 0 otherwise
int isWhiteSpace(char ch)
{
    if(ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        return 1;
    return 0;
}
//returns 1 if a character is a valid character, 0 otherwise
int isValidChar(char ch)
{
    return (isALetter(ch) || isADigit(ch) || isASpecialSymbol(ch) || isWhiteSpace(ch));
}

//returns the token_type value of the character
int getTokenValue(char lexeme[])
{
    int i;
    for(i = 0; i < 15; i++)
    {
        //the current word is reserved word
        if(strcmp(lexeme, word[i]) == 0)
        {
            return wsym[i];
        }
    }
    return identsym;
}

//files to print to
FILE *lexemetable;
FILE *lexemelist;

void printToken(token_t *token)
{
    //prints to the lexemetable
    fprintf(lexemetable, "%s\t%d\n", token->lexeme, token->class);

    //prints to the lexemelist
    fprintf(lexemelist, "%d ", token->class);
    if(token->class == identsym || token->class == numbersym)
    {
        fprintf(lexemelist, "%s ", token->lexeme);
    }

    //reset the token
    int i;
    for(i = 0; i < 11; i++)
        token->lexeme[i] = '\0';
    token->class = -1;
}

void errorNameTooLong()
{
    fprintf(lexemelist, "LEX ERROR: Name too long");
    fprintf(lexemetable, "LEX ERROR: Name too long");
    printf("LEX ERROR: Name too long");
    fclose(lexemelist);
    fclose(lexemetable);
    exit(1);
}
void errorNumberTooLong()
{
    fprintf(lexemelist, "LEX ERROR: Number too long");
    fprintf(lexemetable, "LEX ERROR: Number too long");
    printf("LEX ERROR: Number too long");
    fclose(lexemelist);
    fclose(lexemetable);
    exit(1);
}
void errorVariableDoesNotStartWithLetter()
{
    fprintf(lexemelist, "LEX ERROR: Variable does not start with a letter");
    fprintf(lexemetable, "LEX ERROR: Variable does not start with a letter");
    printf("LEX ERROR: Variable does not start with a letter");
    fclose(lexemelist);
    fclose(lexemetable);
    exit(1);
}
void errorInvalidChar()
{
    fprintf(lexemelist, "LEX ERROR: Invalid Character");
    fprintf(lexemetable, "LEX ERROR: Invalid Character");
    printf("LEX ERROR: Invalid Character");
    fclose(lexemelist);
    fclose(lexemetable);
    exit(1);
}


//the function for seperating the tokens
int seperateTokens(char code[])
{
    //creates the two output files
    lexemetable = fopen("lexemetable.txt", "w");
    fprintf(lexemetable, "lexeme\ttoken type\n");
    lexemelist = fopen("lexemelist.txt", "w");

    //from the slides, the special symbols corresponding values
    int ssym[256];
    ssym['+'] = plussym; ssym['-'] = minussym; ssym['*'] = multsym;
    ssym['/'] = slashsym; ssym['('] = lparentsym; ssym[')'] = rparentsym;
    ssym['='] = eqlsym; ssym[','] = commasym; ssym['.'] = periodsym;
    ssym['#'] = neqsym; ssym['<'] = lessym; ssym['>'] = gtrsym;
    ssym['$'] = leqsym; ssym['%'] = geqsym; ssym[';'] = semicolonsym;

    //initializes token
    token_t token;
    token.class = -1;
    int i;
    for(i = 0; i < 11; i++)
        token.lexeme[i] = '\0';

    int index = 0;
    /*
    state 0: beginning state
    state token_type: coresponds with the other state
    state 50: a colon state (a colon is read in, so the next char must be =, if not an error occurs)
    */
    int state = 0;

    int codeLength = strlen(code);

    while(index < codeLength)
    {
        char currChar = code[index];

        //sends error if an invalid character is input
        if(!isValidChar(currChar))
        {
            errorInvalidChar();
        }

        switch(state)
        {
            //start state
            case 0:
            {
                if(isADigit(currChar))
                {
                    //move to NUM state
                    state = numbersym;
                }
                else if(isALetter(currChar))
                {
                    //move to ID state
                    state = identsym;
                }
                else if(isASpecialSymbol(currChar))
                {
                    //move to colon state
                    if(currChar == ':')
                    {
                        state = 50;
                        //this occurs if ':' is the final character
                        if(index == codeLength-1)
                        {
                            errorInvalidChar();
                        }

                    }
                    //move to Special Symbol state
                    else
                        state = ssym[currChar];
                }
                else if(isWhiteSpace(currChar))
                {
                    //if it is white space, move to the next character
                    break;
                }
                //add the character to the lexeme
                token.lexeme[strlen(token.lexeme)] = currChar;
                break;
            }
            //identifier state
            case identsym:
            {
                //The identifier is finished, print and go back to the begining state
                if(isASpecialSymbol(currChar) || isWhiteSpace(currChar))
                {
                    token.class = state;
                    printToken(&token);
                    state = 0;

                    //index is decremented so that the current character is read again
                    index--;
                }
                //you read a number or letter and you are at the length limit
                else if(strlen(token.lexeme) == cmax)
                {
                    errorNameTooLong();
                }
                //you read in a number or a letter, it either becomes a reserved word or identifier
                else
                {
                    token.lexeme[strlen(token.lexeme)] = currChar;
                    state = getTokenValue(token.lexeme);
                }
                break;
            }
            //number state
            case numbersym:
            {
                //the current character is a digit, add it to the lexeme
                if(isADigit(currChar))
                {
                    //number is longer than 5 digits
                    if(strlen(token.lexeme) > 5)
                    {
                        errorNumberTooLong();
                    }
                    //add the digit to the lexeme
                    token.lexeme[strlen(token.lexeme)] = currChar;
                }
                //the current character is a letter, this is a lexical error
                else if(isALetter(currChar))
                {
                    errorVariableDoesNotStartWithLetter();
                }
                //The number is finished, go back to the begining state
                else
                {
                    token.class = state;
                    printToken(&token);
                    state = 0;

                    //index is decremented so that the current character is read again
                    index--;
                }
                break;
            }

            //reserved words cases
            case nulsym:
            case oddsym:
            case beginsym:
            case endsym:
            case ifsym:
            case thensym:
            case whilesym:
            case dosym:
            case callsym:
            case constsym:
            case varsym:
            case procsym:
            case writesym:
            case readsym:
            case elsesym:
            {
                //it is not longer a reserved word, it is an identifier
                if(isADigit(currChar) || isALetter(currChar))
                {
                    //add the character to the lexeme
                    token.lexeme[strlen(token.lexeme)] = currChar;
                    state = identsym;
                }
                //reserve word is the token, print and go back to the begining state
                else
                {
                    token.class = state;
                    printToken(&token);
                    state = 0;

                    //index is decremented so that the current character is read again
                    index--;
                }
                break;
            }

            //special character cases
            case plussym:
            case minussym:
            case multsym:
            case slashsym:
            case eqlsym:
            case neqsym:
            case leqsym:
            case geqsym:
            case lparentsym:
            case rparentsym:
            case commasym:
            case semicolonsym:
            case periodsym:
            case becomessym:
            {
                token.class = state;
                printToken(&token);
                state = 0;

                //index is decremented so that the current character is read again
                index--;
                break;
            }
            case lessym:
            {
                //this is "<="
                if(currChar == '=')
                {
                    token.lexeme[strlen(token.lexeme)] = currChar;
                    state = leqsym;
                }
                //this is "<>"
                else if(currChar == '>')
                {
                    token.lexeme[strlen(token.lexeme)] = currChar;
                    state = neqsym;
                }
                // '<' is the token, print and go back to the begining state
                else
                {
                    token.class = state;
                    printToken(&token);
                    state = 0;

                    //index is decremented so that the current character is read again
                    index--;
                }
                break;
            }
            case gtrsym:
            {
                //this is ">="
                if(currChar == '=')
                {
                    token.lexeme[strlen(token.lexeme)] = currChar;
                    state = geqsym;
                }
                // '>' is the token, print and go back to the begining state
                else
                {
                    token.class = state;
                    printToken(&token);
                    state = 0;

                    //index is decremented so that the current character is read again
                    index--;
                }
                break;
            }
            //the colon
            case 50:
            {
                //this is ":="
                if(currChar == '=')
                {
                    token.lexeme[strlen(token.lexeme)] = currChar;
                    state = becomessym;
                }
                // ':' is the token, but it is invalid
                else
                {
                    errorInvalidChar();
                }
                break;
            }
        }
        //move to the next character
        index++;
    }
    if(state != 0)
    {
        //prints the final token
        token.class = state;
        printToken(&token);
    }

    fclose(lexemetable);
    fclose(lexemelist);
}


//clean text code here

//Constants go here
#define MAX_CODE_SIZE 2000

FILE *ifp, *ofp;

char codeArray[MAX_CODE_SIZE];

void removeTabsNewline(int size)
{
   int i;

   for(i = 0; i < size; i++)
   {
      if(codeArray[i] == '\t' || codeArray[i] == '\n')
      {
         codeArray[i] = ' ';
      }
   }
}

void printCleanInput(int size)
{
   int i;

   for(i = 0; i < size; i++)
   {
      fprintf(ofp,"%c",codeArray[i]);
   }
}

void removeComments(int size)
{
   int i;
   int halt = 0;

   for(i = 0; i < size; i++)
   {
      if( codeArray[i] == '/' )
      {
         if(i+1 < size  && codeArray[i+1] == '*')
         {
            codeArray[i] = ' ';
            codeArray[i+1] = ' ';
            i += 2;
            while(!halt && i < size)
            {
               if(codeArray[i] == '*')
               {
                  if(codeArray[i+1] == '/')
                  {
                     codeArray[i] = ' ';
                     i++;
                     codeArray[i] = ' ';
                     i++;
                     halt = 1;
                  }
                  else
                  {
                     codeArray[i] = ' ';
                     i++;
                     codeArray[i] = ' ';
                     i++;
                  }
               }
               else
               {
                  codeArray[i] = ' ';
                  i++;
               }
            }
            halt = 0;
         }
         else
         {
            //nothing
         }
      }
      else
      {
         //nothing
      }
      //for
   }
}

int readInput(void)
{
   int halt = 0;
   char c;
   int i = 0;

    while(fscanf(ifp,"%c",&c) != EOF)
   {
      codeArray[i] = c;

      i++;
   }

   return i;
}

int scannermain()
{
    //reads the input and removes comments
    ifp = fopen("input.txt","r");
    ofp = fopen("cleaninput.txt","w");

    int size = readInput();

    removeComments(size);

    printCleanInput(size);

    removeTabsNewline(size);

    fclose(ifp);
    fclose(ofp);

    //uses the code without the comments and tokenizes it
    seperateTokens(codeArray);
}

