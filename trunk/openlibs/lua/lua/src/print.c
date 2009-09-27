/*
** $Id: print.c,v 1.55a 2006/05/31 13:30:05 lhf Exp $
** print bytecodes
** See Copyright Notice in lua.h
*/

#include <ctype.h>
#include <stdio.h>

#define luac_c
#define LUA_CORE

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lundump.h"

#define my_printFunction	luaU_print

#define Sizeof(x)	((int)sizeof(x))
#define VOID(p)		((const void*)(p))

static void PrintString(const TString* ts)
{
 const char* s=getstr(ts);
 size_t i,n=ts->tsv.len;
 putchar('"');
 for (i=0; i<n; i++)
 {
  int c=s[i];
  switch (c)
  {
   case '"': my_printF("\\\""); break;
   case '\\': my_printF("\\\\"); break;
   case '\a': my_printF("\\a"); break;
   case '\b': my_printF("\\b"); break;
   case '\f': my_printF("\\f"); break;
   case '\n': my_printF("\\n"); break;
   case '\r': my_printF("\\r"); break;
   case '\t': my_printF("\\t"); break;
   case '\v': my_printF("\\v"); break;
   default:	if (isprint((unsigned char)c))
   			putchar(c);
		else
			my_printF("\\%03u",(unsigned char)c);
  }
 }
 putchar('"');
}

static void PrintConstant(const Proto* f, int i)
{
 const TValue* o=&f->k[i];
 switch (ttype(o))
 {
  case LUA_TNIL:
	my_printF("nil");
	break;
  case LUA_TBOOLEAN:
	my_printF(bvalue(o) ? "true" : "false");
	break;
  case LUA_TNUMBER:
	my_printF(LUA_NUMBER_FMT,nvalue(o));
	break;
  case LUA_TSTRING:
	PrintString(rawtsvalue(o));
	break;
  default:				/* cannot happen */
	my_printF("? type=%d",ttype(o));
	break;
 }
}

static void PrintCode(const Proto* f)
{
 const Instruction* code=f->code;
 int pc,n=f->sizecode;
 for (pc=0; pc<n; pc++)
 {
  Instruction i=code[pc];
  OpCode o=GET_OPCODE(i);
  int a=GETARG_A(i);
  int b=GETARG_B(i);
  int c=GETARG_C(i);
  int bx=GETARG_Bx(i);
  int sbx=GETARG_sBx(i);
  int line=getline(f,pc);
  my_printF("\t%d\t",pc+1);
  if (line>0) my_printF("[%d]\t",line); else my_printF("[-]\t");
  my_printF("%-9s\t",luaP_opnames[o]);
  switch (getOpMode(o))
  {
   case iABC:
    my_printF("%d",a);
    if (getBMode(o)!=OpArgN) my_printF(" %d",ISK(b) ? (-1-INDEXK(b)) : b);
    if (getCMode(o)!=OpArgN) my_printF(" %d",ISK(c) ? (-1-INDEXK(c)) : c);
    break;
   case iABx:
    if (getBMode(o)==OpArgK) my_printF("%d %d",a,-1-bx); else my_printF("%d %d",a,bx);
    break;
   case iAsBx:
    if (o==OP_JMP) my_printF("%d",sbx); else my_printF("%d %d",a,sbx);
    break;
  }
  switch (o)
  {
   case OP_LOADK:
    my_printF("\t; "); PrintConstant(f,bx);
    break;
   case OP_GETUPVAL:
   case OP_SETUPVAL:
    my_printF("\t; %s", (f->sizeupvalues>0) ? getstr(f->upvalues[b]) : "-");
    break;
   case OP_GETGLOBAL:
   case OP_SETGLOBAL:
    my_printF("\t; %s",svalue(&f->k[bx]));
    break;
   case OP_GETTABLE:
   case OP_SELF:
    if (ISK(c)) { my_printF("\t; "); PrintConstant(f,INDEXK(c)); }
    break;
   case OP_SETTABLE:
   case OP_ADD:
   case OP_SUB:
   case OP_MUL:
   case OP_DIV:
   case OP_POW:
   case OP_EQ:
   case OP_LT:
   case OP_LE:
    if (ISK(b) || ISK(c))
    {
     my_printF("\t; ");
     if (ISK(b)) PrintConstant(f,INDEXK(b)); else my_printF("-");
     my_printF(" ");
     if (ISK(c)) PrintConstant(f,INDEXK(c)); else my_printF("-");
    }
    break;
   case OP_JMP:
   case OP_FORLOOP:
   case OP_FORPREP:
    my_printF("\t; to %d",sbx+pc+2);
    break;
   case OP_CLOSURE:
    my_printF("\t; %p",VOID(f->p[bx]));
    break;
   case OP_SETLIST:
    if (c==0) my_printF("\t; %d",(int)code[++pc]);
    else my_printF("\t; %d",c);
    break;
   default:
    break;
  }
  my_printF("\n");
 }
}

#define SS(x)	(x==1)?"":"s"
#define S(x)	x,SS(x)

static void PrintHeader(const Proto* f)
{
 const char* s=getstr(f->source);
 if (*s=='@' || *s=='=')
  s++;
 else if (*s==LUA_SIGNATURE[0])
  s="(bstring)";
 else
  s="(string)";
 my_printF("\n%s <%s:%d,%d> (%d instruction%s, %d bytes at %p)\n",
 	(f->linedefined==0)?"main":"function",s,
	f->linedefined,f->lastlinedefined,
	S(f->sizecode),f->sizecode*Sizeof(Instruction),VOID(f));
 my_printF("%d%s param%s, %d slot%s, %d upvalue%s, ",
	f->numparams,f->is_vararg?"+":"",SS(f->numparams),
	S(f->maxstacksize),S(f->nups));
 my_printF("%d local%s, %d constant%s, %d function%s\n",
	S(f->sizelocvars),S(f->sizek),S(f->sizep));
}

static void PrintConstants(const Proto* f)
{
 int i,n=f->sizek;
 my_printF("constants (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  my_printF("\t%d\t",i+1);
  PrintConstant(f,i);
  my_printF("\n");
 }
}

static void PrintLocals(const Proto* f)
{
 int i,n=f->sizelocvars;
 my_printF("locals (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  my_printF("\t%d\t%s\t%d\t%d\n",
  i,getstr(f->locvars[i].varname),f->locvars[i].startpc+1,f->locvars[i].endpc+1);
 }
}

static void PrintUpvalues(const Proto* f)
{
 int i,n=f->sizeupvalues;
 my_printF("upvalues (%d) for %p:\n",n,VOID(f));
 if (f->upvalues==NULL) return;
 for (i=0; i<n; i++)
 {
  my_printF("\t%d\t%s\n",i,getstr(f->upvalues[i]));
 }
}

void my_printFunction(const Proto* f, int full)
{
 int i,n=f->sizep;
 PrintHeader(f);
 PrintCode(f);
 if (full)
 {
  PrintConstants(f);
  PrintLocals(f);
  PrintUpvalues(f);
 }
 for (i=0; i<n; i++) my_printFunction(f->p[i],full);
}
