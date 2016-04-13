/* This file is part of Kaiju, Copyright 2015 Peter Menzel and Anders Krogh,
 * Kaiju is licensed under the GPLv3, see the file LICENSE. */
static char *opt_indent = "      ";

/* This code is auto-generated by OptionsAndArguments
   The file should be included by the c program (#include filename)
   To read cmd line use

   OPT_read_cmdline(opt_struct, argc, argv);

*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"


#define OPTTYPE_SWITCH 1
#define OPTTYPE_VALUE 2
#define OPTTYPE_ARG 3

#define VARTYPE_int 1
#define VARTYPE_double 2
#define VARTYPE_charS 3
#define VARTYPE_intS 4
#define VARTYPE_doubleS 5
#define VARTYPE_charSS 6


typedef struct {
  int opttype;
  int vartype;
  void *pt;
  int  *count;
  char *names;
  char *text;
} OPT_STRUCT;


/* PRINT AN ERROR MESSAGE AND EXIT */
static void OPT_error(char *msg) {
  if (msg) fprintf(stderr, "OPT_Error: %s\n", msg);
  exit(1);
}

/* Duplicates a string */
static char *OPT_stringdup(const char *s) {
  int l=1+strlen(s);
  char *ret=(char *)malloc(l*sizeof(char));
  strcpy(ret,s);
  return ret;
}

/* Gives one line per variable with values. Lines are preceeded by
   the prefix string (maybe "").
   If verbose !=0 a more descriptive output is obtained (used with
   -help).
*/
static void OPT_print_vars(FILE *fp, OPT_STRUCT *opt, char *prefix, int  verbose) {
  int argi=1;
  int i;
  char *c;

  if (verbose && opt->text) {
      fprintf(fp,"%s\nOptions and arguments:\n\n",opt->text);
  }
  ++opt;
  while (opt->opttype) {
    if (verbose) {
      fprintf(fp,"%s",prefix);
      c = opt->names;
      if (opt->opttype==OPTTYPE_ARG) fprintf(fp,"ARG %d",argi++);
      else { fputc('-',fp); ++c; }
      while (*c) {
	  if (*c=='|' && *(c+1)) fprintf(fp,", -");
	  if (*c!='|') fputc(*c,fp);
	  ++c;
      }
      if (opt->opttype!=OPTTYPE_SWITCH) {
	switch (opt->vartype) {
	case VARTYPE_int: fprintf(fp," (integer)"); break;
        case VARTYPE_double: fprintf(fp," (double)"); break;
        case VARTYPE_charS: fprintf(fp," (string)"); break;
        case VARTYPE_intS: fprintf(fp," (integer array, current size %d)",*opt->count); break;
        case VARTYPE_doubleS: fprintf(fp," (double array, current size %d)",*opt->count); break;
        case VARTYPE_charSS: fprintf(fp," (string array, current size %d)",*opt->count); break;
        }
      }
      fprintf(fp,"\n%s%s\n%s%sValue: ",prefix,opt->text,prefix,opt_indent);
    }
    else {
      fprintf(fp,"%s",prefix);
      c = opt->names+1; while (*c!='|') fputc(*(c++),fp);
      fputc('=',fp);
    }
    if (opt->opttype==OPTTYPE_SWITCH) {
      fprintf(fp,"%s",(*(int *)(opt->pt)?"ON":"OFF"));
    }
    else {
      switch (opt->vartype) {
      case VARTYPE_int:
	fprintf(fp," %d",*(int *)(opt->pt));
	break;
      case VARTYPE_double:
	fprintf(fp," %f",*(double *)(opt->pt));
	break;
      case VARTYPE_charS:
	if (*(char **)(opt->pt)==NULL) fprintf(fp," NULL");
	fprintf(fp," %s",*(char **)(opt->pt));
	break;
      case VARTYPE_intS:
	if (*(int **)(opt->pt)==NULL) fprintf(fp," NULL");
	else for (i=0;i<*opt->count;++i)
	  fprintf(fp," %d", (*(int **)(opt->pt))[i]);
	break;
      case VARTYPE_doubleS:
	if (*(double **)(opt->pt)==NULL) fprintf(fp," NULL");
	else for (i=0;i<*opt->count;++i)
	  fprintf(fp," %f", (*(double **)(opt->pt))[i]);
	break;
      case VARTYPE_charSS:
	if (*(char ***)(opt->pt)==NULL) fprintf(fp," NULL");
	else for (i=0;i<*opt->count;++i)
	  fprintf(fp," %s", (*(char ***)(opt->pt))[i]);
	break;
      }
    }
    fprintf(fp,"\n");
    if (verbose) fprintf(fp,"%s\n",prefix);

    ++opt;
  }
}


static void OPT_help(OPT_STRUCT *opt) {
  OPT_print_vars(stdout, opt, "", 1);
}


/* Read the command line */
static void OPT_read_cmdline(OPT_STRUCT *opt, int argc, char **argv) {
  int argi=1, i, l, n, match;
  OPT_STRUCT *o;
  char argument[100];
  void *pt;

  while (argi<argc) {
    l=match=0;
    o=opt+1;
    /* if option, find match */
    if (argv[argi][0]=='-') {
      while ( o->opttype ) {
	if (o->opttype == OPTTYPE_SWITCH && argv[argi][1]=='n' && argv[argi][2]=='o') l=3;
	else l=1;
        sprintf(argument,"|%s|",argv[argi]+l);
	if (strstr(o->names,argument)) {
//          fprintf(stderr,"Found option %s =~ %s\n",argv[argi],o->names);
	  ++argi;
	  match=1;
	  break;
	}
	++o;
      }
    }
    else { /* Otherwise it is an argument */
      while ( o->opttype ) {
	if ( o->opttype == OPTTYPE_ARG && *(o->count) == 0 ) {
//          fprintf(stderr,"Found argument %s fits %s\n",argv[argi],o->names);
	  match=1;
	  break;
        }
	++o;
      }
    }
    if (!match) {
      fprintf(stderr,"Didn't understand argument %s\n\n",argv[argi]);
      OPT_print_vars(stderr, opt, "", 1);
      OPT_error("\n");
    }
    /* Now o is pointing to the relevant option and argi is the argument to read */
    if (o->opttype == OPTTYPE_SWITCH) {
      if (l==3) *(int *)(o->pt) = 0;
      else *(int *)(o->pt) = 1;
      *(o->count) += 1;
    }
    else {
      if (argi>=argc) OPT_error("Running out of arguments");
      if (o->vartype == VARTYPE_intS || o->vartype == VARTYPE_doubleS || o->vartype == VARTYPE_charSS) {
	// n = *(o->count) = atoi(argv[argi]);
	n = *(o->count);
	if (*(void **)(o->pt)) free(*(void **)(o->pt));
	*(void **)(o->pt) = NULL;
	if (n<=0) OPT_error("Array has zero or negative length");
	if (argi+n>argc) OPT_error("Running out of arguments");
      }
      else *(o->count) += 1;
      switch (o->vartype) {
      case VARTYPE_int:
	*(int *)(o->pt) = atoi(argv[argi++]);
	break;
      case VARTYPE_double:
	*(double *)(o->pt) = atof(argv[argi++]);
	break;
      case VARTYPE_charS:
	if (*(char **)(o->pt) && *(o->count)>1 ) free(*(char **)(o->pt));
	*(char **)(o->pt) = OPT_stringdup(argv[argi++]);
	break;
      case VARTYPE_intS:
	*(int **)(o->pt) = (int *)calloc(n,sizeof(int));
	for (i=0;i<n; ++i, ++argi) (*(int **)(o->pt))[i] = atoi(argv[argi]);
	break;
      case VARTYPE_doubleS:
	*(double **)(o->pt) = (double *)calloc(n,sizeof(double));
	for (i=0;i<n; ++i, ++argi) (*(double **)(o->pt))[i] = atof(argv[argi]);
	break;
      case VARTYPE_charSS:
	*(char ***)(o->pt) = (char **)calloc(n,sizeof(char *));
	for (i=0;i<n; ++i, ++argi) (*(char ***)(o->pt))[i] = OPT_stringdup(argv[argi]);
	break;
      }
    }
  }
}



// static void OPT_read_varfile(OPT_STRUCT *opt,char **argv, int argc) {
// }




static int count_infilename=0;
static char* infilename = NULL;
static int count_outfilename=0;
static char* outfilename = NULL;
static int count_Alphabet=0;
static char* Alphabet = "protein";
static int count_nThreads=0;
static int nThreads = 2;
static int count_length=0;
static double length = 0;
static int count_checkpoint=0;
static int checkpoint = 5;
static int count_caseSens=0;
static int caseSens = 0;
static int count_revComp=0;
static int revComp = 0;
static int count_term=0;
static char* term = "*";
static int count_revsort=0;
static int revsort = 0;
static int count_help=0;
static int help = 0;

static OPT_STRUCT opt_struct[13] = {
	{OPTTYPE_SWITCH,VARTYPE_int,NULL,NULL,NULL,"---\nmkbwt takes a fasta file as argument and calculates the BWT.\nOutput file name given with -o (defaults to the input file name)\n\nExample cmd line\n   mkbwt -a DNA -o outputname infilename.fsa\nor for proteins (default alphabet)\n   mkbwt -o outputname infilename.fsa\nor for some other alphabet\n   mkbwt -a abcdefgHIJK -o outputname infilename.fsa\n\nIt can also take sequences on stdin, in which case you have to give\nthe filesize in millions of letters (rounded up), e.g. -l 3000\ncorresponding to 3 billion letters.\n\nFiles are created with outputname followed by various extensions\n\nSee options below\n---\n"},
	{OPTTYPE_ARG,VARTYPE_charS,(void *)&infilename,&count_infilename,"|infilename|","      Name of an input file (stdin if no file is given, in which case you\n      need to give length)"},
	{OPTTYPE_VALUE,VARTYPE_charS,(void *)&outfilename,&count_outfilename,"|outfilename|o|","      Name of output. Several files with different extensions are produced\n      (if not given, input file name is used)."},
	{OPTTYPE_VALUE,VARTYPE_charS,(void *)&Alphabet,&count_Alphabet,"|Alphabet|a|","      Alphabet used. Must end with the sequence terminator. Instead of alphabet\n      you can specify DNA, RNA or protein, in which case the alphabet is ACGT,\n      ACGU, or ACDEFGHIKLMNPQRSTVWYX"},
	{OPTTYPE_VALUE,VARTYPE_int,(void *)&nThreads,&count_nThreads,"|nThreads|n|","      Number of threads"},
	{OPTTYPE_VALUE,VARTYPE_double,(void *)&length,&count_length,"|length|l|","      Length of concatenated sequence in millions (one decimal, round up).\n      Used when reading from stdin. If file name is given, length is estimated\n      from file size and length needs not be specified."},
	{OPTTYPE_VALUE,VARTYPE_int,(void *)&checkpoint,&count_checkpoint,"|checkpoint|e|","      Exponent for suffix array checkpoints. There is a checkpoint for every\n      2^e points. Value around 5 is a good compromise between speed and space."},
	{OPTTYPE_SWITCH,VARTYPE_int,(void *)&caseSens,&count_caseSens,"|caseSens|c|","      The sequence is read case sensitive"},
	{OPTTYPE_SWITCH,VARTYPE_int,(void *)&revComp,&count_revComp,"|revComp|r|","      Reverse complement sequence. Works only for DNA."},
	{OPTTYPE_VALUE,VARTYPE_charS,(void *)&term,&count_term,"|term|t|","      Terminating symbol (only used for debugging)"},
	{OPTTYPE_SWITCH,VARTYPE_int,(void *)&revsort,&count_revsort,"|revsort|s|","      The termination symbols sorts as reverse sequences. This will make the\n      BWT more compressible."},
	{OPTTYPE_SWITCH,VARTYPE_int,(void *)&help,(void *)&count_help,"|help|h|","      Prints summary of options and arguments"},
	{0,0,NULL,NULL,NULL,NULL}
};

// This is how you call the cmd line parser etc
// int main(int argc, char **argv) {
//     OPT_read_cmdline(opt_struct, argc, argv);
//     if (help) OPT_help(opt_struct);
//     OPT_print_vars(stderr, opt_struct, "# ", 0);
// }