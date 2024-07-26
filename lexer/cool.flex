/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%option noyywrap 
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
int nested = 0;
int string_length = 0;

%}

/*
 * Define names for regular expressions here.
 */
WHITESPACE      [ \t\r\f\v\b]+
IF              [Ii][Ff]
FI              [Ff][Ii]
ELSE            [Ee][Ll][Ss][Ee]
CLASS           [Cc][Ll][Aa][Ss][Ss]
DARROW          "=>"
LET             [Ll][Ee][Tt]
INHERITS        [Ii][Nn][Hh][Ee][Rr][Ii][Tt][Ss]
IN              [Ii][Nn]
LOOP            [Ll][Oo][Oo][Pp]
POOL            [Pp][Oo][Oo][Ll]
THEN            [Tt][Hh][Ee][Nn]
WHILE           [Ww][Hh][Ii][Ll][Ee]
CASE            [Cc][Aa][Ss][Ee]
ESAC            [Ee][Ss][Aa][Cc]
OF              [Oo][Ff]
NEW             [Nn][Ee][Ww]
ISVOID          [Ii][Ss][Vv][Oo][Ii][Dd]
ASSIGN          "<-"
NOT             [Nn][Oo][Tt]
LE              "<="
TRUE            t[Rr][Uu][Ee]
FALSE           f[Aa][Ll][Ss][Ee]
TYPEID          [A-Z][A-Za-z0-9_]*
OBJECTID        [a-z][A-Za-z0-9_]*
STRINGOPEN      \"
%x COMMENT
%x STRING
%%


 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */

\n            { curr_lineno++; }
{WHITESPACE}  { }
{IF}          { return IF; }
{FI}          { return FI; }
{ELSE}        { return ELSE; }
{CLASS}       { return CLASS; }
{DARROW}      { return DARROW; }
{LET}         { return LET; }
{INHERITS}    { return INHERITS; }
{IN}          { return IN; }
{LOOP}        { return LOOP; }
{POOL}        { return POOL; }
{THEN}        { return THEN; }
{WHILE}       { return WHILE; }
{CASE}        { return CASE; }
{ESAC}        { return ESAC; }
{OF}          { return OF; }
{NEW}         { return NEW; }
{ISVOID}      { return ISVOID; }
{ASSIGN}      { return ASSIGN; }
{NOT}         { return NOT; }
{LE}          { return LE; }
{TRUE}        { yylval.boolean = 1; return (BOOL_CONST); }
{FALSE}       { yylval.boolean = 0; return (BOOL_CONST); }
{TYPEID}      { yylval.symbol = idtable.add_string(yytext); return (TYPEID); }
{OBJECTID}    { yylval.symbol = stringtable.add_string(yytext); return (OBJECTID); }
{STRINGOPEN}  { string_length = 0; BEGIN (STRING); }
[0-9]+        { yylval.symbol = inttable.add_string(yytext); return (INT_CONST); }
"."           { return '.'; }
"-"           { return '-'; }
"+"           { return '+'; }
"("           { return '('; }
")"           { return ')'; }
"{"           { return '{'; }
"}"           { return '}'; }
":"           { return ':'; }
";"           { return ';'; }
","           { return ','; }
"'"           { return '\''; }
"<"           { return '<'; }
"="           { return '='; }
"~"           { return '~'; }
"@"           { return '@'; }
"*"           { return '*'; }
"/"           { return '/'; }

<INITIAL>{
  "--".*      { }
  "(*"        { nested++; BEGIN COMMENT; }
  "*)"        { yylval.error_msg = "Unmatched *)"; return ERROR; }
  .           { yylval.error_msg = strdup(yytext); return ERROR; }
}
<COMMENT>{
  "*)"        { nested--;
                if(nested == 0){
                  BEGIN INITIAL;
                }
              }
  "(*"        { nested++; }
  \n          { curr_lineno++; }
  <<EOF>>     { yylval.error_msg = "EOF in comment"; BEGIN INITIAL; return ERROR; }
  .           { }
}
<STRING>{
  {STRINGOPEN}  { 
                  BEGIN INITIAL; 
                  if(string_length > MAX_STR_CONST-1)
                  { memset(string_buf, 0, sizeof(string_buf));
                    yylval.error_msg = "String constant too long"; 
                    BEGIN INITIAL; return ERROR;
                  }
                  else
                  {
                    yylval.symbol = stringtable.add_string(string_buf);
                    memset(string_buf, 0, sizeof(string_buf));
                    return (STR_CONST);
                  }
                }

  <<EOF>>       { memset(string_buf, 0, sizeof(string_buf));
                  yylval.error_msg = "EOF in string constant"; 
                  BEGIN INITIAL; return ERROR; }
  \n            { memset(string_buf, 0, sizeof(string_buf)); 
                  yylval.error_msg = "Unterminated string constant"; 
                  curr_lineno++; BEGIN INITIAL; return ERROR;}
  \0            { memset(string_buf, 0, sizeof(string_buf)); 
                  yylval.error_msg = "String contains null character"; 
                  BEGIN INITIAL; return ERROR;}
  \\n           { if(string_length > MAX_STR_CONST-1)
                  { yylval.error_msg = "String constant too long"; 
                    memset(string_buf, 0, sizeof(string_buf));
                    BEGIN INITIAL; return ERROR; }
                  else
                  { string_buf[string_length++] = '\n'; } 
                }
  \\t           { if(string_length > MAX_STR_CONST-1)
                  { yylval.error_msg = "String constant too long"; 
                    memset(string_buf, 0, sizeof(string_buf));
                    BEGIN INITIAL; return ERROR; }
                  else
                  { string_buf[string_length++] = '\t'; } 
                }
  \\f           { if(string_length > MAX_STR_CONST-1)
                  { yylval.error_msg = "String constant too long"; 
                    memset(string_buf, 0, sizeof(string_buf));
                    BEGIN INITIAL; return ERROR; }
                  else
                  { string_buf[string_length++] = '\f'; } 
                }
  \\b           { if(string_length > MAX_STR_CONST-1)
                  { yylval.error_msg = "String constant too long"; 
                    memset(string_buf, 0, sizeof(string_buf));
                    BEGIN INITIAL; return ERROR; }
                  else
                  { string_buf[string_length++] = '\b'; } 
                }
  \\[^btnf]     { string_buf[string_length++] = yytext[1]; curr_lineno++;}
  .             { string_buf[string_length++] = *yytext; }
}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */

  /*
   *  NAPOMENE:
   *   -Punjenje stringa uz pomoć *string_buf_ptr++ = *yytext; dovodi do Segmentation fault pogreške.
   *    Problem je zaobiđen upotrebom string_buf[string_length++] = *yytext; koji sadrži brojač karaktera.
   *    Resetiranje buffera se odvija uz pomoć memset(string_buf, 0, sizeof(string_buf)); metode, koja postavlja 
   *    buffer na prvi element niza.
   *   -U testnim datotekama je navedeno da veličina stringa ne smije biti veća od 1024 elementa, što je zabilježeno 
   *    uz provjeru if(string_length > MAX_STR_CONST-1).
   *   -Od novih varijabli, dodane su nested (tipa int) i string_length (tipa int). Prva, nested prati je li došlo do
   *    ugnježđenja komentara, te ako joj je vrijednost 0, izlazi iz komentara. Druga, string_length se puni prilikom 
   *    unosa karaktera u buffer. 
   *
   */

  /*
   *  OBRADA GREŠAKA:
   *   -Pronađene pogreške se prosljeđuju parseru, bez ispisivanja, uz token ERROR.
   *   -String sa ASCII znakom za novi redak javlja grešku "Unterminated string constant".
   *   -String koji je prevelike duljine javlja grešku "String constant too long".
   *   -Nevažeći znakovi javljaju grešku " String contains null character".
   *   -Kraj stringa označen je s pojavom znaka dvostrukih navodnika (", tj. {STRINGOPEN}) ili s ulaskom u sljedeći redak.
   *   -Nailaskom na EOF bez zatvaranja komentara javlja se greška "EOF in comment".
   *   -Nailaskom na EOF bez zatvaranja stringa javlja se greška "EOF in string constant".
   *   -Nailaskom na "*)" izvan komentara javlja se greška "Unmatched *)".
   */  

%%