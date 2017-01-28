/*  Pawn compiler - Error message system
 *  In fact a very simple system, using only 'panic mode'.
 *
 *  Copyright (c) ITB CompuPhase, 1997-2006
 *
 *  This software is provided "as-is", without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1.  The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *  2.  Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *  3.  This notice may not be removed or altered from any source distribution.
 *
 *  Version: $Id: sc5.c 3579 2006-06-06 13:35:29Z thiadmer $
 */
#include <assert.h>
#if defined	__WIN32__ || defined _WIN32 || defined __MSDOS__
  #include <io.h>
#endif
#if defined LINUX || defined __GNUC__
  #include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>     /* ANSI standardized variable argument list functions */
#include <string.h>
#if defined FORTIFY
  #include <alloc/fortify.h>
#endif
#include "sc.h"

static char *errmsg[] = {
/*001*/  "expected token: \"%s\", but found \"%s\"\n",
/*002*/  "only a single statement (or expression) can follow each \"case\"\n",
/*003*/  "declaration of a local variable must appear in a compound block\n",
/*004*/  "function \"%s\" is not implemented\n",
/*005*/  "function may not have arguments\n",
/*006*/  "must be assigned to an array\n",
/*007*/  "operator cannot be redefined\n",
/*008*/  "must be a constant expression; assumed zero\n",
/*009*/  "invalid array size (negative, zero or out of bounds)\n",
/*010*/  "invalid function or declaration\n",
/*011*/  "invalid outside functions\n",
/*012*/  "invalid function call, not a valid address\n",
/*013*/  "no entry point (no public functions)\n",
/*014*/  "invalid statement; not in switch\n",
/*015*/  "\"default\" case must be the last case in switch statement\n",
/*016*/  "multiple defaults in \"switch\"\n",
/*017*/  "undefined symbol \"%s\"\n",
/*018*/  "initialization data exceeds declared size\n",
/*019*/  "not a label: \"%s\"\n",
/*020*/  "invalid symbol name \"%s\"\n",
/*021*/  "symbol already defined: \"%s\"\n",
/*022*/  "must be lvalue (non-constant)\n",
/*023*/  "array assignment must be simple assignment\n",
/*024*/  "\"break\" or \"continue\" is out of context\n",
/*025*/  "function heading differs from prototype\n",
/*026*/  "no matching \"#if...\"\n",
/*027*/  "invalid character constant\n",
/*028*/  "invalid subscript (not an array or too many subscripts): \"%s\"\n",
/*029*/  "invalid expression, assumed zero\n",
/*030*/  "compound statement not closed at the end of file (started at line %d)\n",
/*031*/  "unknown directive\n",
/*032*/  "array index out of bounds (variable \"%s\")\n",
/*033*/  "array must be indexed (variable \"%s\")\n",
/*034*/  "argument does not have a default value (argument %d)\n",
/*035*/  "argument type mismatch (argument %d)\n",
/*036*/  "empty statement\n",
/*037*/  "invalid string (possibly non-terminated string)\n",
/*038*/  "extra characters on line\n",
/*039*/  "constant symbol has no size\n",
/*040*/  "duplicate \"case\" label (value %d)\n",
/*041*/  "invalid ellipsis, array size is not known\n",
/*042*/  "invalid combination of class specifiers\n",
/*043*/  "character constant exceeds range for packed string\n",
/*044*/  "positional parameters must precede all named parameters\n",
/*045*/  "too many function arguments\n",
/*046*/  "unknown array size (variable \"%s\")\n",
/*047*/  "array sizes do not match, or destination array is too small\n",
/*048*/  "array dimensions do not match\n",
/*049*/  "invalid line continuation\n",
/*050*/  "invalid range\n",
/*051*/  "invalid subscript, use \"[ ]\" operators on major dimensions\n",
/*052*/  "multi-dimensional arrays must be fully initialized\n",
/*053*/  "exceeding maximum number of dimensions\n",
/*054*/  "unmatched closing brace (\"}\")\n",
/*055*/  "start of function body without function header\n",
/*056*/  "arrays, local variables and function arguments cannot be public (variable \"%s\")\n",
/*057*/  "unfinished expression before compiler directive\n",
/*058*/  "duplicate argument; same argument is passed twice\n",
/*059*/  "function argument may not have a default value (variable \"%s\")\n",
/*060*/  "multiple \"#else\" directives between \"#if ... #endif\"\n",
/*061*/  "\"#elseif\" directive follows an \"#else\" directive\n",
/*062*/  "number of operands does not fit the operator\n",
/*063*/  "function result tag of operator \"%s\" must be \"%s\"\n",
/*064*/  "cannot change predefined operators\n",
/*065*/  "function argument may only have a single tag (argument %d)\n",
/*066*/  "function argument may not be a reference argument or an array (argument \"%s\")\n",
/*067*/  "variable cannot be both a reference and an array (variable \"%s\")\n",
/*068*/  "invalid rational number precision in #pragma\n",
/*069*/  "rational number format already defined\n",
/*070*/  "rational number support was not enabled\n",
/*071*/  "user-defined operator must be declared before use (function \"%s\")\n",
/*072*/  "\"sizeof\" operator is invalid on \"function\" symbols\n",
/*073*/  "function argument must be an array (argument \"%s\")\n",
/*074*/  "#define pattern must start with an alphabetic character\n",
/*075*/  "input line too long (after substitutions)\n",
/*076*/  "syntax error in the expression, or invalid function call\n",
/*077*/  "malformed UTF-8 encoding, or corrupted file: %s\n",
/*078*/  "function uses both \"return\" and \"return <value>\"\n",
/*079*/  "inconsistent return types (array & non-array)\n",
/*080*/  "unknown symbol, or not a constant symbol (symbol \"%s\")\n",
/*081*/  "cannot take a tag as a default value for an indexed array parameter (symbol \"%s\")\n",
/*082*/  "user-defined operators and native functions may not have states\n",
/*083*/  "a function or variable may only belong to a single automaton (symbol \"%s\")\n",
/*084*/  "state conflict: one of the states is already assigned to another implementation (symbol \"%s\")\n",
/*085*/  "no states are defined for symbol \"%s\"\n",
/*086*/  "unknown automaton \"%s\"\n",
/*087*/  "unknown state \"%s\" for automaton \"%s\"\n",
/*088*/  "public variables and local variables may not have states (symbol \"%s\")\n",
/*089*/  "state variables may not be initialized (symbol \"%s\")\n",
/*090*/  "public functions may not return arrays (symbol \"%s\")\n",
/*091*/  "ambiguous constant; tag override is required (symbol \"%s\")\n",
/*092*/  "static enums may not be declared in a compound block\n"
};

static char *fatalmsg[] = {
/*100*/  "cannot read from file: \"%s\"\n",
/*101*/  "cannot write to file: \"%s\"\n",
/*102*/  "table overflow: \"%s\"\n",
          /* table can be: loop table
           *               literal table
           *               staging buffer
           *               option table (response file)
           *               peephole optimizer table
           */
/*103*/  "insufficient memory\n",
/*104*/  "invalid assembler instruction \"%s\"\n",
/*105*/  "numeric overflow, exceeding capacity\n",
/*106*/  "compiled script exceeds the maximum memory size (%ld bytes)\n",
/*107*/  "too many error messages on one line\n",
/*108*/  "codepage mapping file not found\n",
/*109*/  "invalid path: \"%s\"\n",
/*110*/  "assertion failed: %s\n",
/*111*/  "user error: %s\n"
};

static char *warnmsg[] = {
/*200*/  "symbol \"%s\" is truncated to %d characters\n",
/*201*/  "redefinition of constant/macro (symbol \"%s\")\n",
/*202*/  "number of arguments does not match definition\n",
/*203*/  "symbol is never used: \"%s\"\n",
/*204*/  "symbol is assigned a value that is never used: \"%s\"\n",
/*205*/  "redundant code: constant expression is zero\n",
/*206*/  "redundant test: constant expression is non-zero\n",
/*207*/  "unknown #pragma\n",
/*208*/  "function with tag result used before definition, forcing reparse\n",
/*209*/  "function \"%s\" should return a value\n",
/*210*/  "possible use of symbol before initialization: \"%s\"\n",
/*211*/  "possibly unintended assignment\n",
/*212*/  "possibly unintended bitwise operation\n",
/*213*/  "tag mismatch\n",
/*214*/  "possibly a \"const\" array argument was intended: \"%s\"\n",
/*215*/  "expression has no effect\n",
/*216*/  "nested comment\n",
/*217*/  "loose indentation\n",
/*218*/  "old style prototypes used with optional semicolumns\n",
/*219*/  "local variable \"%s\" shadows a variable at a preceding level\n",
/*220*/  "expression with tag override must appear between parentheses\n",
/*221*/  "label name \"%s\" shadows tag name\n",
/*222*/  "number of digits exceeds rational number precision\n",
/*223*/  "redundant \"sizeof\": argument size is always 1 (symbol \"%s\")\n",
/*224*/  "indeterminate array size in \"sizeof\" expression (symbol \"%s\")\n",
/*225*/  "unreachable code\n",
/*226*/  "a variable is assigned to itself (symbol \"%s\")\n",
/*227*/  "more initiallers than enum fields\n",
/*228*/  "length of initialler exceeds size of the enum field\n",
/*229*/  "index tag mismatch (symbol \"%s\")\n",
/*230*/  "no implementation for state \"%s\" in function \"%s\", no fall-back\n",
/*231*/  "state specification on forward declaration is ignored\n",
/*232*/  "output file is written, but with compact encoding disabled\n",
/*233*/  "state variable \"%s\" shadows a global variable\n",
/*234*/  "function is deprecated (symbol \"%s\") %s\n",
/*235*/  "public function lacks forward declaration (symbol \"%s\")\n",
/*236*/  "unknown parameter in substitution (incorrect #define pattern)\n",
/*237*/  "user warning: %s\n"
};

#define NUM_WARNINGS    (sizeof warnmsg / sizeof warnmsg[0])
static struct s_warnstack {
  unsigned char disable[(NUM_WARNINGS + 7) / 8]; /* 8 flags in a char */
  struct s_warnstack *next;
} warnstack;

static int errflag;
static int errstart;    /* line number at which the instruction started */
static int errline;     /* forced line number for the error message */

/*  error
 *
 *  Outputs an error message (note: msg is passed optionally).
 *  If an error is found, the variable "errflag" is set and subsequent
 *  errors are ignored until lex() finds a semicolumn or a keyword
 *  (lex() resets "errflag" in that case).
 *
 *  Global references: inpfname   (reffered to only)
 *                     fline      (reffered to only)
 *                     fcurrent   (reffered to only)
 *                     errflag    (altered)
 */
SC_FUNC int error(int number,...)
{
static char *prefix[3]={ "error", "fatal error", "warning" };
static int lastline,errorcount;
static short lastfile;
  char *msg,*pre;
  va_list argptr;

  /* errflag is reset on each semicolon.
   * In a two-pass compiler, an error should not be reported twice. Therefore
   * the error reporting is enabled only in the second pass (and only when
   * actually producing output). Fatal errors may never be ignored.
   */
  if ((errflag || sc_status!=statWRITE) && (number<100 || number>=200))
    return 0;

  /* also check for disabled warnings */
  if (number>=200) {
    int index=(number-200)/8;
    int mask=1 << ((number-200)%8);
    if ((warnstack.disable[index] & mask)!=0)
      return 0;
  } /* if */

  if (number<100){
    msg=errmsg[number-1];
    pre=prefix[0];
    errflag=TRUE;       /* set errflag (skip rest of erroneous expression) */
    errnum++;
  } else if (number<200){
    msg=fatalmsg[number-100];
    pre=prefix[1];
    errnum++;           /* a fatal error also counts as an error */
  } else {
    msg=warnmsg[number-200];
    pre=prefix[2];
    warnnum++;
  } /* if */

  assert(errstart<=fline);
  if (errline>0)
    errstart=errline;
  else
    errline=fline;
  assert(errstart<=errline);
  va_start(argptr,number);
  if (strlen(errfname)==0) {
    int start= (errstart==errline) ? -1 : errstart;
    if (pc_error(number,msg,inpfname,start,errline,argptr)) {
      if (outf!=NULL) {
        pc_closeasm(outf,TRUE);
        outf=NULL;
      } /* if */
      longjmp(errbuf,3);        /* user abort */
    } /* if */
  } else {
    FILE *fp=fopen(errfname,"a");
    if (fp!=NULL) {
      if (errstart>=0 && errstart!=errline)
        fprintf(fp,"%s(%d -- %d) : %s %03d: ",inpfname,errstart,errline,pre,number);
      else
        fprintf(fp,"%s(%d) : %s %03d: ",inpfname,errline,pre,number);
      vfprintf(fp,msg,argptr);
      fclose(fp);
    } /* if */
  } /* if */
  va_end(argptr);

  if (number>=100 && number<200 || errnum>25){
    if (strlen(errfname)==0) {
      va_start(argptr,number);
      pc_error(0,"\nCompilation aborted.\n\n",NULL,0,0,argptr);
      va_end(argptr);
    } /* if */
    if (outf!=NULL) {
      pc_closeasm(outf,TRUE);
      outf=NULL;
    } /* if */
    longjmp(errbuf,2);          /* fatal error, quit */
  } /* if */

  errline=-1;
  /* check whether we are seeing many errors on the same line */
  if ((errstart<0 && lastline!=fline) || lastline<errstart || lastline>fline || fcurrent!=lastfile)
    errorcount=0;
  lastline=fline;
  lastfile=fcurrent;
  if (number<200)
    errorcount++;
  if (errorcount>=3)
    error(107);         /* too many error/warning messages on one line */

  return 0;
}

SC_FUNC void errorset(int code,int line)
{
  switch (code) {
  case sRESET:
    errflag=FALSE;      /* start reporting errors */
    break;
  case sFORCESET:
    errflag=TRUE;       /* stop reporting errors */
    break;
  case sEXPRMARK:
    errstart=fline;     /* save start line number */
    break;
  case sEXPRRELEASE:
    errstart=-1;        /* forget start line number */
    errline=-1;
    break;
  case sSETPOS:
    errline=line;
    break;
  } /* switch */
}

/* pc_enablewarning()
 * Enables or disables a warning (errors cannot be disabled).
 * Initially all warnings are enabled. The compiler does this by setting bits
 * for the *disabled* warnings and relying on the array to be zero-initialized.
 *
 * Parameter enable can be:
 *  o  0 for disable
 *  o  1 for enable
 *  o  2 for toggle
 */
int pc_enablewarning(int number,int enable)
{
  int index;
  unsigned char mask;

  if (number<200)
    return FALSE;       /* errors and fatal errors cannot be disabled */
  number -= 200;
  if (number>=NUM_WARNINGS)
    return FALSE;

  index=number/8;
  mask=(unsigned char)(1 << (number%8));
  switch (enable) {
  case 0:
    warnstack.disable[index] |= mask;
    break;
  case 1:
    warnstack.disable[index] &= (unsigned char)~mask;
    break;
  case 2:
    warnstack.disable[index] ^= mask;
    break;
  } /* switch */

  return TRUE;
}

/* pc_pushwarnings()
 * Saves currently disabled warnings, used to implement #pragma warning push
 */
int pc_pushwarnings()
{
  void *p;
  p=calloc(sizeof(struct s_warnstack),1);
  if (p==NULL) {
    error(103); /* insufficient memory */
    return FALSE;
  }
  memmove(p,&warnstack,sizeof(struct s_warnstack));
  warnstack.next=p;
  return TRUE;
}

/* pc_popwarnings()
 * This function is the reverse of pc_pushwarnings()
 */
int pc_popwarnings()
{
  void *p;
  if (warnstack.next==NULL)
    return FALSE; /* nothing to do */
  p=warnstack.next;
  memmove(&warnstack,p,sizeof(struct s_warnstack));
  free(p);
  return TRUE;
}

