/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ParserContext {
  Query * ssql;
  size_t select_length;
  size_t condition_length;
  size_t from_length;
  size_t value_length;
  size_t record_length;
  Value values[MAX_NUM];
  Insert_Record records[MAX_NUM];
  Condition conditions[MAX_NUM];
  CompOp comp;
	char id[MAX_NUM];
} ParserContext;

//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  ParserContext *context = (ParserContext *)(yyget_extra(scanner));
  query_reset(context->ssql);
  context->ssql->flag = SCF_ERROR;
  context->condition_length = 0;
  context->from_length = 0;
  context->select_length = 0;
  context->value_length = 0;
  context->ssql->sstr.errors = NULL;
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 129 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SEMICOLON = 258,
    CREATE = 259,
    DROP = 260,
    TABLE = 261,
    TABLES = 262,
    INDEX = 263,
    SELECT = 264,
    DESC = 265,
    ASC = 266,
    SHOW = 267,
    SYNC = 268,
    INSERT = 269,
    DELETE = 270,
    UPDATE = 271,
    LBRACE = 272,
    RBRACE = 273,
    COMMA = 274,
    TRX_BEGIN = 275,
    TRX_COMMIT = 276,
    TRX_ROLLBACK = 277,
    INT_T = 278,
    STRING_T = 279,
    FLOAT_T = 280,
    DATE_T = 281,
    TEXT_T = 282,
    HELP = 283,
    EXIT = 284,
    DOT = 285,
    INTO = 286,
    VALUES = 287,
    FROM = 288,
    WHERE = 289,
    AND = 290,
    SET = 291,
    ON = 292,
    LOAD = 293,
    DATA = 294,
    INFILE = 295,
    EQ = 296,
    LT = 297,
    GT = 298,
    LE = 299,
    GE = 300,
    NE = 301,
    AGGR_COUNT = 302,
    AGGR_MAX = 303,
    AGGR_MIN = 304,
    AGGR_AVG = 305,
    UNIQUE = 306,
    NOT = 307,
    NULLTOKEN = 308,
    NULLABLE = 309,
    ISTOKEN = 310,
    ORDER = 311,
    GROUP = 312,
    BY = 313,
    INNER = 314,
    JOIN = 315,
    ADD_OP = 316,
    SUB_OP = 317,
    DIV_OP = 318,
    NUMBER = 319,
    FLOAT = 320,
    ID = 321,
    DATE = 322,
    PATH = 323,
    SSS = 324,
    STAR = 325,
    STRING_V = 326
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 129 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  struct _ExpressionTree *exp_tree;
  struct _ExpressionNode *exp_node;
  char *string;
  int number;
  float floats;
  char *position;

#line 265 "yacc_sql.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   246

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  125
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  238

#define YYUNDEFTOK  2
#define YYMAXUTOK   326


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   167,   167,   169,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   193,   198,   203,   209,   215,   221,   227,   233,   239,
     246,   253,   255,   259,   261,   264,   270,   277,   286,   288,
     292,   303,   314,   320,   328,   331,   332,   333,   335,   336,
     339,   348,   365,   367,   370,   375,   377,   382,   385,   388,
     393,   397,   403,   406,   412,   422,   432,   450,   452,   457,
     463,   465,   470,   477,   479,   484,   491,   493,   498,   506,
     509,   512,   518,   524,   530,   535,   540,   542,   550,   554,
     560,   563,   569,   573,   579,   583,   590,   595,   603,   610,
     618,   628,   638,   639,   640,   641,   645,   647,   651,   653,
     657,   659,   665,   667,   671,   673,   678,   686,   687,   688,
     689,   690,   691,   692,   693,   697
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEMICOLON", "CREATE", "DROP", "TABLE",
  "TABLES", "INDEX", "SELECT", "DESC", "ASC", "SHOW", "SYNC", "INSERT",
  "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T",
  "TEXT_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM", "WHERE",
  "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT", "LE",
  "GE", "NE", "AGGR_COUNT", "AGGR_MAX", "AGGR_MIN", "AGGR_AVG", "UNIQUE",
  "NOT", "NULLTOKEN", "NULLABLE", "ISTOKEN", "ORDER", "GROUP", "BY",
  "INNER", "JOIN", "ADD_OP", "SUB_OP", "DIV_OP", "NUMBER", "FLOAT", "ID",
  "DATE", "PATH", "SSS", "STAR", "STRING_V", "$accept", "commands",
  "command", "exit", "help", "sync", "begin", "commit", "rollback",
  "drop_table", "show_tables", "desc_table", "create_index", "index_type",
  "id_list", "index_attr_id", "drop_index", "create_table",
  "attr_def_list", "attr_def", "number", "type", "ID_get", "insert",
  "record_list", "record", "value_list", "value", "sign", "delete",
  "update", "select", "order", "group", "group_id_list", "order_list",
  "order_type", "select_attr", "attr", "attr_list", "add_sub_expression",
  "add_or_sub", "mul_div_expression", "mul_or_div", "atom_expression",
  "aggr", "aggr_op", "rel_list", "inner_join", "on", "where",
  "condition_list", "condition", "comOp", "load_data", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326
};
# endif

#define YYPACT_NINF (-210)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -210,   104,  -210,    -3,    85,   -15,   -59,    30,    11,    12,
      35,     8,    74,    95,    98,   100,   102,    67,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,  -210,    41,  -210,   103,    44,
      46,    33,  -210,  -210,  -210,  -210,  -210,  -210,  -210,    91,
    -210,  -210,  -210,  -210,   -37,    89,   108,    20,  -210,   -46,
    -210,  -210,   106,   125,   126,  -210,    64,    65,   105,  -210,
    -210,  -210,  -210,  -210,   109,   123,    78,   142,   143,    -2,
      84,  -210,  -210,    87,    23,  -210,    33,  -210,  -210,    33,
      -4,  -210,  -210,   124,   131,    92,    97,   101,   132,  -210,
    -210,  -210,  -210,   149,   108,   -46,  -210,   152,   153,   155,
      33,   170,   133,   144,  -210,   157,   137,   111,   112,   120,
    -210,  -210,  -210,    90,   161,    93,   146,  -210,    90,   176,
     101,   165,  -210,  -210,  -210,  -210,  -210,   -13,   167,   149,
     127,   131,   166,   155,   183,  -210,  -210,  -210,  -210,  -210,
    -210,   136,    33,    33,  -210,   131,   128,   157,   186,   129,
     138,  -210,   130,  -210,   134,   135,    90,   172,   161,  -210,
    -210,    20,   146,   192,   194,  -210,  -210,  -210,   180,  -210,
    -210,   182,   149,   141,   147,   166,  -210,  -210,  -210,  -210,
    -210,  -210,   130,   184,   168,   140,   150,   201,  -210,   182,
     204,    33,   120,     0,   145,  -210,  -210,  -210,   146,  -210,
     148,   151,  -210,    -1,  -210,    48,   190,  -210,  -210,   154,
     191,   156,  -210,  -210,    77,   158,  -210,   190,   191,     1,
    -210,  -210,   159,   191,    77,  -210,   191,  -210
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,    31,     0,    62,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    20,
      19,    14,    15,    16,    17,     9,    10,    11,    12,    13,
       8,     5,     7,     6,     4,    18,     0,    32,     0,     0,
       0,    62,   102,   104,   105,   103,    61,    90,    91,    97,
      60,    59,    82,    99,     0,     0,    86,    84,    63,    89,
      93,    85,     0,     0,     0,    23,     0,     0,     0,    24,
      25,    26,    22,    21,     0,     0,     0,     0,     0,     0,
       0,    57,    58,     0,    62,    83,    62,    95,    94,    62,
      62,    29,    28,     0,   112,     0,     0,     0,     0,    27,
      36,    96,    98,   106,    86,    88,    92,     0,     0,     0,
      62,     0,     0,     0,    50,    38,     0,     0,     0,   108,
      87,   100,   101,    62,    52,     0,   114,    64,    62,     0,
       0,     0,    45,    46,    47,    48,    49,    41,     0,   106,
       0,   112,    55,     0,     0,   117,   118,   119,   120,   121,
     122,   123,    62,    62,   113,   112,     0,    38,     0,     0,
       0,    43,     0,   107,     0,    70,    62,     0,    52,    51,
     124,   116,   114,     0,     0,    39,    37,    44,     0,    42,
      35,    33,   106,     0,    67,    55,    54,    53,   115,    65,
     125,    40,     0,     0,   110,     0,     0,     0,    56,    33,
       0,    62,   108,    73,     0,    66,    34,    30,   114,   109,
       0,     0,    71,    79,   111,    73,    73,    81,    80,     0,
      76,     0,    74,    72,    79,     0,    68,    73,    76,    79,
      75,    69,     0,    76,    79,    77,    76,    78
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,
    -210,  -210,  -210,  -210,    13,    21,  -210,  -210,    58,    86,
    -210,  -210,  -210,  -210,    50,    76,    36,  -102,  -210,  -210,
    -210,  -210,  -210,  -210,  -171,  -153,  -209,  -210,   139,   122,
      -5,   -56,   160,  -210,     7,  -210,  -210,  -121,    25,  -210,
    -133,  -166,  -148,  -210,  -210
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    38,   193,   181,    29,    30,   131,   115,
     178,   137,   116,    31,   144,   124,   167,    53,    54,    32,
      33,    34,   197,   184,   212,   226,   220,    55,    56,    85,
     125,    58,    59,    89,    60,    61,    62,   119,   141,   202,
     111,   154,   126,   152,    35
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      57,    86,    41,    36,   159,   172,   188,    63,   165,   217,
     218,   217,   218,    41,    65,   228,   101,    87,   163,   210,
     233,   142,   173,    86,    88,   236,   155,    81,    82,   219,
     211,   232,    42,    43,    44,    45,    79,    64,    46,   160,
      41,   161,   214,    66,   222,   223,    47,    48,    37,    46,
      41,    49,    50,   208,    51,    52,   230,    47,    48,    47,
      48,   194,    49,    50,   185,    51,   107,   210,    67,    86,
      42,    43,    44,    45,    68,   231,    46,    69,   221,    57,
     235,    47,    48,   237,    47,    48,    46,   217,   218,    49,
      50,    39,    51,    40,    47,    48,   106,   108,    70,    49,
      50,    71,    51,    72,     2,    73,    74,    75,     3,     4,
      77,    76,    78,     5,     6,    86,     7,     8,     9,    10,
      11,    80,    83,    90,    12,    13,    14,    84,    91,    92,
      93,    94,    15,    16,   145,   146,   147,   148,   149,   150,
      97,    95,    17,    46,    98,    99,   100,   171,   151,    96,
     102,    47,    48,   103,    47,    48,   109,    50,   112,    51,
     132,   133,   134,   135,   136,   110,   113,   114,   118,   117,
     121,   122,   123,   127,   128,   129,   130,   138,   139,   140,
     143,   153,   156,   158,   162,   166,   169,   164,   170,   176,
     186,   179,   183,   177,   174,   189,   180,   190,   191,   195,
     182,   192,   200,   196,   205,   201,   203,   207,   204,   210,
     225,   213,   206,   199,   215,   175,   157,   216,   187,   168,
     224,   198,   227,   104,   229,   234,   120,   209,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   105
};

static const yytype_int16 yycheck[] =
{
       5,    57,    17,     6,    17,   153,   172,    66,   141,    10,
      11,    10,    11,    17,     3,   224,    18,    63,   139,    19,
     229,   123,   155,    79,    70,   234,   128,    64,    65,    30,
      30,    30,    47,    48,    49,    50,    41,     7,    53,    52,
      17,    54,   208,    31,   215,   216,    61,    62,    51,    53,
      17,    66,    67,   201,    69,    70,   227,    61,    62,    61,
      62,   182,    66,    67,   166,    69,    70,    19,    33,   125,
      47,    48,    49,    50,    66,   228,    53,     3,    30,    84,
     233,    61,    62,   236,    61,    62,    53,    10,    11,    66,
      67,     6,    69,     8,    61,    62,    89,    90,     3,    66,
      67,     3,    69,     3,     0,     3,    39,    66,     4,     5,
      66,     8,    66,     9,    10,   171,    12,    13,    14,    15,
      16,    30,    33,    17,    20,    21,    22,    19,     3,     3,
      66,    66,    28,    29,    41,    42,    43,    44,    45,    46,
      17,    36,    38,    53,    66,     3,     3,   152,    55,    40,
      66,    61,    62,    66,    61,    62,    32,    67,    66,    69,
      23,    24,    25,    26,    27,    34,    69,    66,    19,    37,
      18,    18,    17,     3,    41,    31,    19,    66,    66,    59,
      19,    35,     6,    18,    17,    19,     3,    60,    52,     3,
      18,    53,    57,    64,    66,     3,    66,     3,    18,    58,
      66,    19,    18,    56,     3,    37,    66,     3,    58,    19,
      19,    66,   199,   192,    66,   157,   130,    66,   168,   143,
      66,   185,    66,    84,    66,    66,   104,   202,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    73,     0,     4,     5,     9,    10,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    38,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    88,
      89,    95,   101,   102,   103,   126,     6,    51,    85,     6,
       8,    17,    47,    48,    49,    50,    53,    61,    62,    66,
      67,    69,    70,    99,   100,   109,   110,   112,   113,   114,
     116,   117,   118,    66,     7,     3,    31,    33,    66,     3,
       3,     3,     3,     3,    39,    66,     8,    66,    66,   112,
      30,    64,    65,    33,    19,   111,   113,    63,    70,   115,
      17,     3,     3,    66,    66,    36,    40,    17,    66,     3,
       3,    18,    66,    66,   110,   114,   116,    70,   116,    32,
      34,   122,    66,    69,    66,    91,    94,    37,    19,   119,
     111,    18,    18,    17,    97,   112,   124,     3,    41,    31,
      19,    90,    23,    24,    25,    26,    27,    93,    66,    66,
      59,   120,    99,    19,    96,    41,    42,    43,    44,    45,
      46,    55,   125,    35,   123,    99,     6,    91,    18,    17,
      52,    54,    17,   119,    60,   122,    19,    98,    97,     3,
      52,   112,   124,   122,    66,    90,     3,    64,    92,    53,
      66,    87,    66,    57,   105,    99,    18,    96,   123,     3,
       3,    18,    19,    86,   119,    58,    56,   104,    98,    87,
      18,    37,   121,    66,    58,     3,    86,     3,   124,   120,
      19,    30,   106,    66,   123,    66,    66,    10,    11,    30,
     108,    30,   106,   106,    66,    19,   107,    66,   108,    66,
     106,   107,    30,   108,    66,   107,   108,   107
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    72,    73,    73,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    85,    86,    86,    87,    88,    89,    90,    90,
      91,    91,    91,    91,    92,    93,    93,    93,    93,    93,
      94,    95,    96,    96,    97,    98,    98,    99,    99,    99,
      99,    99,   100,   100,   101,   102,   103,   104,   104,   104,
     105,   105,   105,   106,   106,   106,   107,   107,   107,   108,
     108,   108,   109,   109,   110,   110,   111,   111,   112,   112,
     113,   113,   114,   114,   115,   115,   116,   116,   116,   116,
     117,   117,   118,   118,   118,   118,   119,   119,   120,   120,
     121,   121,   122,   122,   123,   123,   124,   125,   125,   125,
     125,   125,   125,   125,   125,   126
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     4,     3,     3,
      11,     0,     1,     0,     3,     1,     4,     8,     0,     3,
       5,     2,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     7,     0,     3,     4,     0,     3,     2,     2,     1,
       1,     1,     0,     1,     5,     8,    10,     0,     5,     7,
       0,     4,     6,     0,     3,     5,     0,     4,     6,     0,
       1,     1,     1,     2,     1,     1,     0,     3,     3,     1,
       1,     1,     3,     1,     1,     1,     3,     1,     3,     1,
       4,     4,     1,     1,     1,     1,     0,     3,     0,     6,
       0,     3,     0,     3,     0,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     2,     8
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 21:
#line 193 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1629 "yacc_sql.tab.c"
    break;

  case 22:
#line 198 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1637 "yacc_sql.tab.c"
    break;

  case 23:
#line 203 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1645 "yacc_sql.tab.c"
    break;

  case 24:
#line 209 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1653 "yacc_sql.tab.c"
    break;

  case 25:
#line 215 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1661 "yacc_sql.tab.c"
    break;

  case 26:
#line 221 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1669 "yacc_sql.tab.c"
    break;

  case 27:
#line 227 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1678 "yacc_sql.tab.c"
    break;

  case 28:
#line 233 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1686 "yacc_sql.tab.c"
    break;

  case 29:
#line 239 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1695 "yacc_sql.tab.c"
    break;

  case 30:
#line 247 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1704 "yacc_sql.tab.c"
    break;

  case 32:
#line 255 "yacc_sql.y"
             {
        create_index_unique_init(&CONTEXT->ssql->sstr.create_index);
    }
#line 1712 "yacc_sql.tab.c"
    break;

  case 35:
#line 264 "yacc_sql.y"
       {
        create_index_attr_init(&CONTEXT->ssql->sstr.create_index,(yyvsp[0].string));
    }
#line 1720 "yacc_sql.tab.c"
    break;

  case 36:
#line 271 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1729 "yacc_sql.tab.c"
    break;

  case 37:
#line 278 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1741 "yacc_sql.tab.c"
    break;

  case 39:
#line 288 "yacc_sql.y"
                                   {    }
#line 1747 "yacc_sql.tab.c"
    break;

  case 40:
#line 293 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number), 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
#line 1762 "yacc_sql.tab.c"
    break;

  case 41:
#line 304 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[0].number), 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
#line 1777 "yacc_sql.tab.c"
    break;

  case 42:
#line 314 "yacc_sql.y"
                                   {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-2].number), 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
#line 1788 "yacc_sql.tab.c"
    break;

  case 43:
#line 320 "yacc_sql.y"
                              {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
#line 1799 "yacc_sql.tab.c"
    break;

  case 44:
#line 328 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1805 "yacc_sql.tab.c"
    break;

  case 45:
#line 331 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1811 "yacc_sql.tab.c"
    break;

  case 46:
#line 332 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1817 "yacc_sql.tab.c"
    break;

  case 47:
#line 333 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1823 "yacc_sql.tab.c"
    break;

  case 48:
#line 335 "yacc_sql.y"
                { (yyval.number)=DATES; }
#line 1829 "yacc_sql.tab.c"
    break;

  case 49:
#line 336 "yacc_sql.y"
                {(yyval.number)=TEXTS;}
#line 1835 "yacc_sql.tab.c"
    break;

  case 50:
#line 340 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1844 "yacc_sql.tab.c"
    break;

  case 51:
#line 349 "yacc_sql.y"
                {
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string), CONTEXT->records, CONTEXT->record_length);

      //临时变量清零
      CONTEXT->value_length=0;
      CONTEXT->record_length=0;
    }
#line 1864 "yacc_sql.tab.c"
    break;

  case 54:
#line 370 "yacc_sql.y"
                                   {
        record_init(&CONTEXT->records[CONTEXT->record_length++],CONTEXT->values,CONTEXT->value_length);
        CONTEXT->value_length = 0;
    }
#line 1873 "yacc_sql.tab.c"
    break;

  case 56:
#line 377 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1881 "yacc_sql.tab.c"
    break;

  case 57:
#line 382 "yacc_sql.y"
               {
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number),(yyvsp[-1].number));
		}
#line 1889 "yacc_sql.tab.c"
    break;

  case 58:
#line 385 "yacc_sql.y"
               {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats),(yyvsp[-1].number));
		}
#line 1897 "yacc_sql.tab.c"
    break;

  case 59:
#line 388 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1906 "yacc_sql.tab.c"
    break;

  case 60:
#line 393 "yacc_sql.y"
          {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1915 "yacc_sql.tab.c"
    break;

  case 61:
#line 397 "yacc_sql.y"
                   {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 1923 "yacc_sql.tab.c"
    break;

  case 62:
#line 403 "yacc_sql.y"
                {
        (yyval.number) = ADD;
    }
#line 1931 "yacc_sql.tab.c"
    break;

  case 63:
#line 406 "yacc_sql.y"
                 {
        (yyval.number) = (yyvsp[0].number);
    }
#line 1939 "yacc_sql.tab.c"
    break;

  case 64:
#line 413 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1951 "yacc_sql.tab.c"
    break;

  case 65:
#line 423 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1963 "yacc_sql.tab.c"
    break;

  case 66:
#line 433 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-6].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
#line 1983 "yacc_sql.tab.c"
    break;

  case 68:
#line 452 "yacc_sql.y"
                                        {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), NULL, (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }
#line 1993 "yacc_sql.tab.c"
    break;

  case 69:
#line 457 "yacc_sql.y"
                                               {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), (yyvsp[-4].string), (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }
#line 2003 "yacc_sql.tab.c"
    break;

  case 71:
#line 465 "yacc_sql.y"
                                {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2013 "yacc_sql.tab.c"
    break;

  case 72:
#line 470 "yacc_sql.y"
                                       {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2023 "yacc_sql.tab.c"
    break;

  case 74:
#line 479 "yacc_sql.y"
                                 {
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2033 "yacc_sql.tab.c"
    break;

  case 75:
#line 484 "yacc_sql.y"
                                        {
		GroupAttr group_attr;
		group_attr_init(&group_attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2043 "yacc_sql.tab.c"
    break;

  case 77:
#line 493 "yacc_sql.y"
                                     {
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), NULL, (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
#line 2053 "yacc_sql.tab.c"
    break;

  case 78:
#line 498 "yacc_sql.y"
                                            {
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), (yyvsp[-4].string), (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
#line 2063 "yacc_sql.tab.c"
    break;

  case 79:
#line 506 "yacc_sql.y"
                {
        (yyval.number) = ASC_T; // 默认升序
    }
#line 2071 "yacc_sql.tab.c"
    break;

  case 80:
#line 509 "yacc_sql.y"
          {
        (yyval.number) = ASC_T; //上一行的ASC表示token 这一行的ASC表示的是enum里的类型
    }
#line 2079 "yacc_sql.tab.c"
    break;

  case 81:
#line 512 "yacc_sql.y"
           {
        (yyval.number) = DESC_T; // 
    }
#line 2087 "yacc_sql.tab.c"
    break;

  case 82:
#line 518 "yacc_sql.y"
         {  
        RelAttr attr;
        relation_attr_init(&attr, NULL, "*", NULL);
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2097 "yacc_sql.tab.c"
    break;

  case 83:
#line 524 "yacc_sql.y"
                       {

    }
#line 2105 "yacc_sql.tab.c"
    break;

  case 84:
#line 530 "yacc_sql.y"
                       {
        RelAttr attr;
        relation_attr_init(&attr, NULL, NULL, (yyvsp[0].exp_node));
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
    }
#line 2115 "yacc_sql.tab.c"
    break;

  case 85:
#line 535 "yacc_sql.y"
           {

    }
#line 2123 "yacc_sql.tab.c"
    break;

  case 87:
#line 542 "yacc_sql.y"
                           {

    }
#line 2131 "yacc_sql.tab.c"
    break;

  case 88:
#line 550 "yacc_sql.y"
                                                     {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init((yyval.exp_node), 1, (yyvsp[-2].exp_node), (yyvsp[-1].number), (yyvsp[0].exp_node), 0, NULL, NULL, 0);
    }
#line 2140 "yacc_sql.tab.c"
    break;

  case 89:
#line 554 "yacc_sql.y"
                         {
        (yyval.exp_node) = (yyvsp[0].exp_node);
    }
#line 2148 "yacc_sql.tab.c"
    break;

  case 90:
#line 560 "yacc_sql.y"
           {
        (yyval.number) = ADD;
    }
#line 2156 "yacc_sql.tab.c"
    break;

  case 91:
#line 563 "yacc_sql.y"
             {
        (yyval.number) = SUB;
    }
#line 2164 "yacc_sql.tab.c"
    break;

  case 92:
#line 569 "yacc_sql.y"
                                                  {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init((yyval.exp_node), 1, (yyvsp[-2].exp_node), (yyvsp[-1].number), (yyvsp[0].exp_node), 0, NULL, NULL, 0);
    }
#line 2173 "yacc_sql.tab.c"
    break;

  case 93:
#line 573 "yacc_sql.y"
                      {
        (yyval.exp_node) = (yyvsp[0].exp_node);
    }
#line 2181 "yacc_sql.tab.c"
    break;

  case 94:
#line 579 "yacc_sql.y"
         {
        /* 记录操作符 */
        (yyval.number) = MUL;
    }
#line 2190 "yacc_sql.tab.c"
    break;

  case 95:
#line 583 "yacc_sql.y"
             {
        /* 记录操作符 */
        (yyval.number) = DIV;
    }
#line 2199 "yacc_sql.tab.c"
    break;

  case 96:
#line 590 "yacc_sql.y"
                                    {
        (yyval.exp_node) = (yyvsp[-1].exp_node);
        (yyval.exp_node)->isBracket = 1;
    }
#line 2208 "yacc_sql.tab.c"
    break;

  case 97:
#line 595 "yacc_sql.y"
           {
        /* 记录 列名 */
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, NULL, (yyvsp[0].string), NULL);
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 0, attr, NULL, 0);
	}
#line 2220 "yacc_sql.tab.c"
    break;

  case 98:
#line 603 "yacc_sql.y"
              {
        /* 记录表名 */
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string), NULL);
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 0, attr, NULL, 0);
	}
#line 2232 "yacc_sql.tab.c"
    break;

  case 99:
#line 610 "yacc_sql.y"
            {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        Value *value_ = &CONTEXT->values[CONTEXT->value_length - 1];
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 1, NULL, value_, 0);
    }
#line 2242 "yacc_sql.tab.c"
    break;

  case 100:
#line 618 "yacc_sql.y"
                                   {
            ExpressionNode node;
            RelAttr rel_attr;
            relation_attr_init(&rel_attr,NULL,"*",NULL);
            expression_node_init(&node,0,NULL,0,NULL,0,&rel_attr,NULL,0 );
			AggrAttr attr;
			/*第二个参数refer to AggrType */
			aggr_attr_init(&attr, (yyvsp[-3].number), &node);
			selects_append_aggr_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2257 "yacc_sql.tab.c"
    break;

  case 101:
#line 628 "yacc_sql.y"
                                               {
            ExpressionNode *node = (yyvsp[-1].exp_node);
            AggrAttr attr;
			/*第二个参数refer to AggrType */
			aggr_attr_init(&attr, (yyvsp[-3].number), node);
			selects_append_aggr_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2269 "yacc_sql.tab.c"
    break;

  case 102:
#line 638 "yacc_sql.y"
                   { (yyval.number)=1; }
#line 2275 "yacc_sql.tab.c"
    break;

  case 103:
#line 639 "yacc_sql.y"
                    { (yyval.number)=2; }
#line 2281 "yacc_sql.tab.c"
    break;

  case 104:
#line 640 "yacc_sql.y"
                   { (yyval.number)=3; }
#line 2287 "yacc_sql.tab.c"
    break;

  case 105:
#line 641 "yacc_sql.y"
                   { (yyval.number)=4; }
#line 2293 "yacc_sql.tab.c"
    break;

  case 107:
#line 647 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2301 "yacc_sql.tab.c"
    break;

  case 109:
#line 653 "yacc_sql.y"
                                               {
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
		}
#line 2309 "yacc_sql.tab.c"
    break;

  case 111:
#line 659 "yacc_sql.y"
                                      {

		}
#line 2317 "yacc_sql.tab.c"
    break;

  case 113:
#line 667 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2325 "yacc_sql.tab.c"
    break;

  case 115:
#line 673 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2333 "yacc_sql.tab.c"
    break;

  case 116:
#line 678 "yacc_sql.y"
                                                {
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, (yyvsp[-2].exp_node), (yyvsp[0].exp_node));
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
#line 2343 "yacc_sql.tab.c"
    break;

  case 117:
#line 686 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2349 "yacc_sql.tab.c"
    break;

  case 118:
#line 687 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2355 "yacc_sql.tab.c"
    break;

  case 119:
#line 688 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2361 "yacc_sql.tab.c"
    break;

  case 120:
#line 689 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2367 "yacc_sql.tab.c"
    break;

  case 121:
#line 690 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2373 "yacc_sql.tab.c"
    break;

  case 122:
#line 691 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2379 "yacc_sql.tab.c"
    break;

  case 123:
#line 692 "yacc_sql.y"
                  { CONTEXT->comp = IS; }
#line 2385 "yacc_sql.tab.c"
    break;

  case 124:
#line 693 "yacc_sql.y"
                      { CONTEXT->comp = ISNOT; }
#line 2391 "yacc_sql.tab.c"
    break;

  case 125:
#line 698 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2400 "yacc_sql.tab.c"
    break;


#line 2404 "yacc_sql.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 703 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
