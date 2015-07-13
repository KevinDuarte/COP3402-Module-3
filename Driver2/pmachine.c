/* COP4302 Systems Software PM/0 Virtual Machine
   Coded by Nestor J. Maysonet
   University of Central Florida
   PID: 2536902
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Constants
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

FILE* ifp, *ofp; //Global input and output file pointers

typedef struct {
   int op;  //opcode
   int l;   // L
   int m;   // M
   int line;//line of code
   }instruction;
//Code array
instruction code[MAX_CODE_LENGTH];
//Stack array
int stack[MAX_STACK_HEIGHT];
//Current line of code
int line = 0;
//Lexical level array
int lexi[3];
//Lexi array pointer
int lexilvl = 0;
//CPU globals
int bp = 1;          //base of current AR
int sp = 0;          //top of stack
int pc = 0;          //program counter
instruction* ir;     //instruction register
int halt = 0;        //halt condition
char opString [5];   //string for opcode

//FUNCTION PROTOTYPES//
void fetch(void);
void execute(void);
int codeStore(void);
void oprSwitch(int m);
void printCode(int count);
void stringSwitch(int op);
void printTrace(void);
void printTraceToConsole(void);
int base(int level, int b);

//Main
int pmachinemain()
{
   ifp = fopen("mcode.txt","r");
   ofp = fopen("stacktrace.txt","w");
   stack[0] = 0;
   stack[1] = 0;
   stack[2] = 0;
   lexi[0] = 0;
    lexi[1] = 0;
    lexi[2] = 0;
    lexi[3] = 0;

   int count = codeStore();

   printCode(count);

   fprintf(ofp,"                      pc   bp   sp   stack\n");
   fprintf(ofp,"Initial values        %2d   %2d   %2d\n",pc,bp,sp);
   fprintf(ofp," ");


   while( halt != 1)
   {
      fetch();

      if(pc == count)
         halt = 1;

      execute();

      printTrace();
      fprintf(ofp," \n ");
   }

   fclose(ifp);
   fclose(ofp);

}

//Fetch Cycle
//An instruction is fetched from the code store
//Then, the program counter is incremented by one

void fetch(void)
{
   ir = &code[pc];
   pc++;
}

//ir.op indicates the operation to be executed
//If ir.op is 02, then ir.m identifies the arithmetic
//or logical instruction to be executed.
void execute(void)
{
   switch(ir->op)
   {
      case 1:        //LIT 0 M
         sp = sp + 1;
         stack[sp] = ir->m;
         strcpy(opString,"lit");
         break;
      case 2:        //OPR 0 M
         oprSwitch(ir->m);
         strcpy(opString,"opr");
         break;
      case 3:        //LOD L M
         sp++;
         stack[sp] = stack[ base(ir->l, bp) + ir->m];
         strcpy(opString,"lod");
         break;
      case 4:        //STO L M
         stack[base(ir->l,bp) + ir->m] = stack[sp];
         sp--;
         strcpy(opString,"sto");
         break;
      case 5:        //CAL L M
         stack[sp + 1] = 0;                  //return value (FV)
         stack[sp + 2] = base(ir->l,bp);     //static link (SL)
         stack[sp + 3] = bp;                 //dynamic link (DL)
         stack[sp + 4] = pc;                 //return address (RA)
         bp = sp + 1;
         pc = ir->m;
         //For printing out correct stack
         lexi[lexilvl] = sp;
         lexilvl++;
         strcpy(opString,"cal");
         break;
      case 6:        //INC 0 M
         sp = sp + ir->m;
         strcpy(opString,"inc");
         break;
      case 7:        //JMP 0 M
         pc = ir->m;
         strcpy(opString,"jmp");
         break;
      case 8:        //JPC
         if( stack[sp] == 0)
         {
            pc = ir->m;
         }
         sp--;
         strcpy(opString,"jpc");
         break;
      case 9:        //SIO - pop & print, read & push, halt
         if( ir->m == 0)
         {
            printf("Output: %d\n\n",stack[sp]);
            sp--;
         }else if(ir-> m == 1)
         {
            sp++;
            printf("Enter value to push to stack: ");
            scanf("%d",&stack[sp]);
            printf("\n");
         }else if(ir->m == 2)
         {
            halt = 1;
         }
         strcpy(opString,"sio");
         break;
   }
}

//Reads ints from input file and stores them into
//the code store which is an array of instruction structs.
int codeStore(void)
{
   int halt = 0;
   int count = 0;
   while(halt != 1)
   {
      fscanf(ifp,"%d",&code[count].op);
      fscanf(ifp,"%d",&code[count].l);
      fscanf(ifp,"%d",&code[count].m);
      code[count].line = count;

      if(code[count].op == 9 && code[count].m == 2)
      {
         halt = 1;
      }
      count++;
   }
   return count;
}

//Switch statement for the OPR instruction
void oprSwitch(int m)
{
   switch(m)
   {
      case 0:           //RET
         lexilvl--;
         sp = bp - 1;
         pc = stack[sp + 4];
         bp = stack[sp + 3];
         break;
      case 1:           //NEG
         stack[sp] = -stack[sp];
         break;
      case 2:           //ADD
         sp--;
         stack[sp] = stack[sp] + stack[sp + 1];
         break;
      case 3:           //SUB
         sp--;
         stack[sp] = stack[sp] - stack[sp + 1];
         break;
      case 4:           //MUL
         sp--;
         stack[sp] = stack[sp]*stack[sp + 1];
         break;
      case 5:           //DIV
         sp--;
         stack[sp] = stack[sp] / stack[sp + 1];
         break;
      case 6:           //ODD
         stack[sp] = stack[sp]%2;
         break;
      case 7:           //MOD
         sp--;
         stack[sp] = stack[sp] % stack[sp + 1];
         break;
      case 8:           //EQL
         sp--;
         stack[sp] = stack[sp] == stack[sp + 1];
         break;
      case 9:           //NEQ
         sp--;
         stack[sp] = stack[sp] != stack[sp + 1];
         break;
      case 10:          //LSS
         sp--;
         stack[sp] = stack[sp] < stack[sp + 1];
         break;
      case 11:          //LEQ
         sp--;
         stack[sp] = stack[sp] <= stack[sp + 1];
         break;
      case 12:          //GTR
         sp--;
         stack[sp] = stack[sp] > stack[sp + 1];
         break;
      case 13:          //GEQ
         sp--;
         stack[sp] = stack[sp] >= stack[sp + 1];
   }
}

//Iterates through code array printing out the code
//that was scanned from the input file
void printCode(int count)
{
   instruction temp;
   int i;

   fprintf(ofp,"Line  OP     L   M\n");

   for(i = 0; i < count; i++)
   {
      temp = code[i];
      stringSwitch(temp.op);
      fprintf(ofp," %2d   %2s   %2d   %2d\n",i,opString,temp.l,temp.m);
   }
}

//Function to handle assigning appropriate string
//for printCode and printTrace. Takes the opcode
//as its only parameter.
void stringSwitch(int op)
{
   switch(op)
   {
      case 1:
         strcpy(opString,"lit");
         break;
      case 2:
         strcpy(opString,"opr");
         break;
      case 3:
         strcpy(opString,"lod");
         break;
      case 4:
         strcpy(opString,"sto");
         break;
      case 5:
         strcpy(opString,"cal");
         break;
      case 6:
         strcpy(opString,"inc");
         break;
      case 7:
         strcpy(opString,"jmp");
         break;
      case 8:
         strcpy(opString,"jpc");
         break;
      case 9:
         strcpy(opString,"sio");
         break;
   }

}

//Function to act as place holder for the print
//statements used to print the stack trace to the
//output file. Uses stringSwitch to get correct op
//string to display. Will print vertical bar to
//separate each Activation Record.
void printTrace(void)
{
   int temp;
   int i;
   int current = 0;

   stringSwitch(ir->op);

   if(pc > 1)
      line = pc -1;

   fprintf(ofp,"%2d   %3s   %2d   %2d   %2d   %2d   %2d   ",
           ir->line,opString,ir->l,ir->m,pc,bp,sp);
   temp = sp;

   for(i = 1; i <= sp; i++)
   {
    if(lexi[current] > 0 && i > lexi[current])
    {
      fprintf(ofp,"|");
      current++;
    }
      fprintf(ofp,"%d ",stack[i]);

   }
}

void printTraceToConsole(void)
{
   int temp;
   int i;
   int current = 0;

   stringSwitch(ir->op);

   if(pc > 1)
      line = pc -1;

   printf("%2d   %3s   %2d   %2d   %2d   %2d   %2d   ",
           ir->line,opString,ir->l,ir->m,pc,bp,sp);
   temp = sp;

   for(i = 1; i <= sp; i++)
   {
    if(lexi[current] > 0 && i > lexi[current])
    {
      printf("|");
      current++;
    }
      printf("%d ",stack[i]);

   }
}

//Compute base of activation record L levels down
int base(int level,int b )
{
   while(level > 0)
   {
      b = stack[b+2];
      level--;
   }
   return b;
}
