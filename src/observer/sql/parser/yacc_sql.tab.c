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
  size_t sub_condition_length; //标识子查询的conditon数量
  size_t from_length;
  size_t value_length;
  size_t sub_value_length;  //标识子查询的value数量
  size_t record_length;
  Value values[MAX_NUM];
  Insert_Record records[MAX_NUM];
  Condition conditions[MAX_NUM];
  Condition sub_conditions[MAX_NUM]; //存放子查询的conditions
  CompOp comp;
  CompOp sub_comp;
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
  context->sub_condition_length = 0;
  context->from_length = 0;
  context->select_length = 0;
  context->value_length = 0;
  context->sub_value_length = 0;
  context->ssql->sstr.errors = NULL;
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 135 "yacc_sql.tab.c"

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
    IN = 319,
    NUMBER = 320,
    FLOAT = 321,
    ID = 322,
    DATE = 323,
    PATH = 324,
    SSS = 325,
    STAR = 326,
    STRING_V = 327
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 136 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  struct _ExpressionTree *exp_tree;
  struct _ExpressionNode *exp_node;
  struct _SimpleSubSelect *s_select;
  char *string;
  int number;
  float floats;
  char *position;

#line 273 "yacc_sql.tab.c"

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
typedef yytype_int16 yy_state_t;

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
#define YYLAST   279

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  133
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  262

#define YYUNDEFTOK  2
#define YYMAXUTOK   327


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
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   176,   176,   178,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   202,   207,   212,   218,   224,   230,   236,   242,   248,
     255,   262,   264,   268,   270,   273,   279,   286,   295,   297,
     301,   312,   323,   329,   337,   340,   341,   342,   344,   345,
     348,   357,   374,   376,   379,   384,   386,   391,   394,   397,
     402,   406,   412,   415,   421,   431,   441,   460,   486,   503,
     512,   514,   519,   525,   527,   532,   539,   541,   546,   553,
     555,   560,   568,   571,   574,   580,   586,   592,   597,   602,
     604,   611,   615,   621,   624,   630,   634,   640,   644,   651,
     656,   661,   669,   676,   684,   694,   704,   705,   706,   707,
     711,   713,   717,   719,   723,   725,   731,   733,   737,   739,
     744,   749,   755,   764,   765,   766,   767,   768,   769,   770,
     771,   772,   773,   777
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
  "INNER", "JOIN", "ADD_OP", "SUB_OP", "DIV_OP", "IN", "NUMBER", "FLOAT",
  "ID", "DATE", "PATH", "SSS", "STAR", "STRING_V", "$accept", "commands",
  "command", "exit", "help", "sync", "begin", "commit", "rollback",
  "drop_table", "show_tables", "desc_table", "create_index", "index_type",
  "id_list", "index_attr_id", "drop_index", "create_table",
  "attr_def_list", "attr_def", "number", "type", "ID_get", "insert",
  "record_list", "record", "value_list", "value", "sign", "delete",
  "update", "select", "sub_select", "sub_select_flag", "sub_select_action",
  "order", "group", "group_id_list", "order_list", "order_type",
  "select_attr", "attr", "attr_list", "add_sub_expression", "add_or_sub",
  "mul_div_expression", "mul_or_div", "atom_expression", "aggr", "aggr_op",
  "rel_list", "inner_join", "on", "where", "condition_list", "condition",
  "comOp", "load_data", YY_NULLPTR
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
     325,   326,   327
};
# endif

#define YYPACT_NINF (-227)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -227,   181,  -227,     3,     4,    33,   -46,    17,    23,    11,
       7,   -18,    72,    74,    99,   105,   114,    79,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,  -227,  -227,    52,  -227,   112,    55,
      56,    92,  -227,  -227,  -227,  -227,  -227,  -227,   109,    98,
    -227,  -227,  -227,  -227,    27,    97,   115,    37,  -227,    26,
    -227,  -227,   116,   128,   136,  -227,    73,    75,   107,  -227,
    -227,  -227,  -227,  -227,   104,   129,    80,   145,   148,     9,
      92,    85,  -227,  -227,    90,    88,  -227,  -227,    92,  -227,
    -227,    92,    -2,  -227,  -227,   132,   127,   100,    95,   101,
     133,  -227,  -227,  -227,    12,  -227,   147,   115,    26,  -227,
     151,   155,   157,   110,   172,   135,   152,  -227,   163,    87,
     117,  -227,   120,   130,  -227,  -227,  -227,   -33,   169,    -6,
     194,    88,   170,   164,  -227,   -33,   186,   101,   180,  -227,
    -227,  -227,  -227,  -227,    -9,   183,   147,   144,   127,   187,
     157,   202,  -227,  -227,  -227,  -227,  -227,  -227,  -227,   143,
     156,  -227,    92,  -227,   184,   110,   110,  -227,   127,   153,
     163,   215,   158,   168,  -227,   159,  -227,   160,   167,   -33,
     210,   169,  -227,  -227,  -227,    37,   162,  -227,    37,   164,
     227,   230,  -227,  -227,  -227,   223,  -227,  -227,   224,   147,
     189,   188,   187,  -227,  -227,   147,  -227,  -227,  -227,  -227,
     159,   232,   205,   178,   190,   248,  -227,   130,   224,   249,
     110,   130,    22,   192,  -227,   127,  -227,  -227,   164,  -227,
     193,   195,  -227,     6,   167,  -227,    66,   234,  -227,  -227,
     196,   235,   188,   197,  -227,  -227,    96,   198,  -227,   237,
     234,   235,     8,  -227,  -227,  -227,   199,   235,    96,  -227,
     235,  -227
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    31,     0,    62,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    20,
      19,    14,    15,    16,    17,     9,    10,    11,    12,    13,
       8,     5,     7,     6,     4,    18,     0,    32,     0,     0,
       0,    62,   106,   108,   109,   107,    61,    93,    94,   101,
      60,    59,    85,   103,     0,     0,    89,    87,    63,    92,
      96,    88,     0,     0,     0,    23,     0,     0,     0,    24,
      25,    26,    22,    21,     0,     0,     0,     0,     0,     0,
      62,     0,    57,    58,     0,    62,    86,    94,    62,    98,
      97,    62,    62,    29,    28,     0,   116,     0,     0,     0,
       0,    27,    36,   100,     0,   102,   110,    89,    91,    95,
       0,     0,     0,    62,     0,     0,     0,    50,    38,     0,
       0,    99,     0,   112,    90,   104,   105,    62,    52,    62,
       0,    62,     0,   118,    64,    62,     0,     0,     0,    45,
      46,    47,    48,    49,    41,     0,   110,     0,   116,    55,
       0,     0,    68,   123,   124,   125,   126,   127,   128,     0,
     129,   131,    62,    67,     0,    62,    62,   117,   116,     0,
      38,     0,     0,     0,    43,     0,   111,     0,    73,    62,
       0,    52,    51,   132,   130,   122,     0,   121,   120,   118,
       0,     0,    39,    37,    44,     0,    42,    35,    33,   110,
       0,    70,    55,    54,    53,   110,   119,    65,   133,    40,
       0,     0,   114,     0,     0,     0,    56,   112,    33,     0,
      62,   112,    76,     0,    66,   116,    34,    30,   118,   113,
       0,     0,    74,    82,    73,   115,    76,    76,    84,    83,
       0,    79,    70,     0,    77,    75,    82,     0,    71,     0,
      76,    79,    82,    69,    78,    72,     0,    79,    82,    80,
      79,    81
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,    38,    47,  -227,  -227,    91,   131,
    -227,  -227,  -227,  -227,    86,   119,    68,  -121,  -227,  -227,
    -227,  -227,   106,  -227,  -227,    30,    39,  -204,  -226,  -168,
    -227,   -78,   171,   -41,   -56,   191,  -227,    24,  -227,  -227,
    -142,  -130,  -227,  -146,  -184,  -153,   142,  -227
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    38,   211,   198,    29,    30,   138,   118,
     195,   144,   119,    31,   151,   128,   180,    53,    54,    32,
      33,    34,   130,   131,   163,   215,   201,   232,   248,   241,
      55,    56,    86,    57,    58,    59,    91,    60,    61,    62,
     123,   148,   221,   114,   167,   133,   162,    35
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      79,    88,   178,   152,   176,   206,   149,   107,   172,    36,
      39,    41,    40,   189,   168,    41,   238,   239,   238,   239,
      46,    63,   190,    88,    64,   255,    65,   103,    47,    87,
     121,   259,   244,   245,   261,    50,   240,    51,   256,   104,
      67,   230,    66,   173,   235,   174,   254,    46,    88,    68,
      41,    46,   231,   164,    37,    47,    48,   212,   202,    47,
      48,    49,    50,   217,    51,    49,    50,   228,    51,   110,
      47,    87,   132,    47,    87,    69,    88,    70,   251,   234,
      42,    43,    44,    45,   257,   230,    46,   225,    79,    89,
     260,   229,    82,    83,    47,    48,   243,    90,    47,    87,
      49,    50,    71,    51,    52,    41,   238,   239,    72,    41,
     139,   140,   141,   142,   143,   109,   111,    73,    74,    75,
      76,   185,    77,    78,   188,   132,    80,   129,    81,    88,
      84,    93,    88,    92,    85,    42,    43,    44,    45,    94,
      95,    46,    96,    97,    98,    46,    99,   100,   101,    47,
      48,   102,   105,    47,    48,    49,    50,   106,    51,    49,
      50,   113,    51,    46,   112,   116,   122,   115,   117,   125,
     120,    47,    48,   126,   127,   134,   135,    49,    50,   132,
      51,     2,   137,   136,   145,     3,     4,   146,   150,   147,
       5,     6,   169,     7,     8,     9,    10,    11,   171,   166,
     175,    12,    13,    14,   177,   182,   179,   183,   184,    15,
      16,   153,   154,   155,   156,   157,   158,   186,   193,    17,
     191,   196,   159,   194,   200,   160,   197,   199,   203,   205,
     207,    47,    87,   208,   161,   153,   154,   155,   156,   157,
     158,   209,   220,   210,   214,   222,   159,   213,   223,   160,
     219,   224,   227,   230,   247,   253,   226,   218,   161,   233,
     236,   192,   237,   246,   250,   252,   258,   204,   170,   181,
     216,   187,   249,   242,   165,     0,     0,     0,   124,   108
};

static const yytype_int16 yycheck[] =
{
      41,    57,   148,     9,   146,   189,   127,    85,    17,     6,
       6,    17,     8,   166,   135,    17,    10,    11,    10,    11,
      53,    67,   168,    79,     7,   251,     3,    18,    61,    62,
      18,   257,   236,   237,   260,    68,    30,    70,    30,    80,
      33,    19,    31,    52,   228,    54,   250,    53,   104,    67,
      17,    53,    30,   131,    51,    61,    62,   199,   179,    61,
      62,    67,    68,   205,    70,    67,    68,   220,    70,    71,
      61,    62,   113,    61,    62,     3,   132,     3,   246,   225,
      47,    48,    49,    50,   252,    19,    53,   217,   129,    63,
     258,   221,    65,    66,    61,    62,    30,    71,    61,    62,
      67,    68,     3,    70,    71,    17,    10,    11,     3,    17,
      23,    24,    25,    26,    27,    91,    92,     3,    39,    67,
       8,   162,    67,    67,   165,   166,    17,    17,    30,   185,
      33,     3,   188,    17,    19,    47,    48,    49,    50,     3,
      67,    53,    67,    36,    40,    53,    17,    67,     3,    61,
      62,     3,    67,    61,    62,    67,    68,    67,    70,    67,
      68,    34,    70,    53,    32,    70,    19,    67,    67,    18,
      37,    61,    62,    18,    17,     3,    41,    67,    68,   220,
      70,     0,    19,    31,    67,     4,     5,    67,    19,    59,
       9,    10,     6,    12,    13,    14,    15,    16,    18,    35,
      17,    20,    21,    22,    60,     3,    19,    64,    52,    28,
      29,    41,    42,    43,    44,    45,    46,    33,     3,    38,
      67,    53,    52,    65,    57,    55,    67,    67,    18,    67,
       3,    61,    62,     3,    64,    41,    42,    43,    44,    45,
      46,    18,    37,    19,    56,    67,    52,    58,    58,    55,
      18,     3,     3,    19,    19,    18,   218,   210,    64,    67,
      67,   170,    67,    67,    67,    67,    67,   181,   137,   150,
     202,   165,   242,   234,   132,    -1,    -1,    -1,   107,    88
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,     0,     4,     5,     9,    10,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    38,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    89,
      90,    96,   102,   103,   104,   130,     6,    51,    86,     6,
       8,    17,    47,    48,    49,    50,    53,    61,    62,    67,
      68,    70,    71,   100,   101,   113,   114,   116,   117,   118,
     120,   121,   122,    67,     7,     3,    31,    33,    67,     3,
       3,     3,     3,     3,    39,    67,     8,    67,    67,   116,
      17,    30,    65,    66,    33,    19,   115,    62,   117,    63,
      71,   119,    17,     3,     3,    67,    67,    36,    40,    17,
      67,     3,     3,    18,   116,    67,    67,   114,   118,   120,
      71,   120,    32,    34,   126,    67,    70,    67,    92,    95,
      37,    18,    19,   123,   115,    18,    18,    17,    98,    17,
     105,   106,   116,   128,     3,    41,    31,    19,    91,    23,
      24,    25,    26,    27,    94,    67,    67,    59,   124,   100,
      19,    97,     9,    41,    42,    43,    44,    45,    46,    52,
      55,    64,   129,   107,   114,   129,    35,   127,   100,     6,
      92,    18,    17,    52,    54,    17,   123,    60,   126,    19,
      99,    98,     3,    64,    52,   116,    33,   105,   116,   128,
     126,    67,    91,     3,    65,    93,    53,    67,    88,    67,
      57,   109,   100,    18,    97,    67,   127,     3,     3,    18,
      19,    87,   123,    58,    56,   108,    99,   123,    88,    18,
      37,   125,    67,    58,     3,   124,    87,     3,   128,   124,
      19,    30,   110,    67,   126,   127,    67,    67,    10,    11,
      30,   112,   109,    30,   110,   110,    67,    19,   111,   108,
      67,   112,    67,    18,   110,   111,    30,   112,    67,   111,
     112,   111
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    74,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    86,    87,    87,    88,    89,    90,    91,    91,
      92,    92,    92,    92,    93,    94,    94,    94,    94,    94,
      95,    96,    97,    97,    98,    99,    99,   100,   100,   100,
     100,   100,   101,   101,   102,   103,   104,   105,   106,   107,
     108,   108,   108,   109,   109,   109,   110,   110,   110,   111,
     111,   111,   112,   112,   112,   113,   113,   114,   114,   115,
     115,   116,   116,   117,   117,   118,   118,   119,   119,   120,
     120,   120,   120,   120,   121,   121,   122,   122,   122,   122,
     123,   123,   124,   124,   125,   125,   126,   126,   127,   127,
     128,   128,   128,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   130
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
       1,     1,     0,     1,     5,     8,    10,     2,     2,     9,
       0,     5,     7,     0,     4,     6,     0,     3,     5,     0,
       4,     6,     0,     1,     1,     1,     2,     1,     1,     0,
       3,     3,     1,     1,     1,     3,     1,     1,     1,     4,
       3,     1,     3,     1,     4,     4,     1,     1,     1,     1,
       0,     3,     0,     6,     0,     3,     0,     3,     0,     3,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     8
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
#line 202 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1656 "yacc_sql.tab.c"
    break;

  case 22:
#line 207 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1664 "yacc_sql.tab.c"
    break;

  case 23:
#line 212 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1672 "yacc_sql.tab.c"
    break;

  case 24:
#line 218 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1680 "yacc_sql.tab.c"
    break;

  case 25:
#line 224 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1688 "yacc_sql.tab.c"
    break;

  case 26:
#line 230 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1696 "yacc_sql.tab.c"
    break;

  case 27:
#line 236 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1705 "yacc_sql.tab.c"
    break;

  case 28:
#line 242 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1713 "yacc_sql.tab.c"
    break;

  case 29:
#line 248 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1722 "yacc_sql.tab.c"
    break;

  case 30:
#line 256 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1731 "yacc_sql.tab.c"
    break;

  case 32:
#line 264 "yacc_sql.y"
             {
        create_index_unique_init(&CONTEXT->ssql->sstr.create_index);
    }
#line 1739 "yacc_sql.tab.c"
    break;

  case 35:
#line 273 "yacc_sql.y"
       {
        create_index_attr_init(&CONTEXT->ssql->sstr.create_index,(yyvsp[0].string));
    }
#line 1747 "yacc_sql.tab.c"
    break;

  case 36:
#line 280 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1756 "yacc_sql.tab.c"
    break;

  case 37:
#line 287 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1768 "yacc_sql.tab.c"
    break;

  case 39:
#line 297 "yacc_sql.y"
                                   {    }
#line 1774 "yacc_sql.tab.c"
    break;

  case 40:
#line 302 "yacc_sql.y"
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
#line 1789 "yacc_sql.tab.c"
    break;

  case 41:
#line 313 "yacc_sql.y"
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
#line 1804 "yacc_sql.tab.c"
    break;

  case 42:
#line 323 "yacc_sql.y"
                                   {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-2].number), 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
#line 1815 "yacc_sql.tab.c"
    break;

  case 43:
#line 329 "yacc_sql.y"
                              {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
#line 1826 "yacc_sql.tab.c"
    break;

  case 44:
#line 337 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1832 "yacc_sql.tab.c"
    break;

  case 45:
#line 340 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1838 "yacc_sql.tab.c"
    break;

  case 46:
#line 341 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1844 "yacc_sql.tab.c"
    break;

  case 47:
#line 342 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1850 "yacc_sql.tab.c"
    break;

  case 48:
#line 344 "yacc_sql.y"
                { (yyval.number)=DATES; }
#line 1856 "yacc_sql.tab.c"
    break;

  case 49:
#line 345 "yacc_sql.y"
                {(yyval.number)=TEXTS;}
#line 1862 "yacc_sql.tab.c"
    break;

  case 50:
#line 349 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1871 "yacc_sql.tab.c"
    break;

  case 51:
#line 358 "yacc_sql.y"
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
#line 1891 "yacc_sql.tab.c"
    break;

  case 54:
#line 379 "yacc_sql.y"
                                   {
        record_init(&CONTEXT->records[CONTEXT->record_length++],CONTEXT->values,CONTEXT->value_length);
        CONTEXT->value_length = 0;
    }
#line 1900 "yacc_sql.tab.c"
    break;

  case 56:
#line 386 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1908 "yacc_sql.tab.c"
    break;

  case 57:
#line 391 "yacc_sql.y"
               {
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number),(yyvsp[-1].number));
		}
#line 1916 "yacc_sql.tab.c"
    break;

  case 58:
#line 394 "yacc_sql.y"
               {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats),(yyvsp[-1].number));
		}
#line 1924 "yacc_sql.tab.c"
    break;

  case 59:
#line 397 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1933 "yacc_sql.tab.c"
    break;

  case 60:
#line 402 "yacc_sql.y"
          {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1942 "yacc_sql.tab.c"
    break;

  case 61:
#line 406 "yacc_sql.y"
                   {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 1950 "yacc_sql.tab.c"
    break;

  case 62:
#line 412 "yacc_sql.y"
                {
        (yyval.number) = ADD;
    }
#line 1958 "yacc_sql.tab.c"
    break;

  case 63:
#line 415 "yacc_sql.y"
                 {
        (yyval.number) = (yyvsp[0].number);
    }
#line 1966 "yacc_sql.tab.c"
    break;

  case 64:
#line 422 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1978 "yacc_sql.tab.c"
    break;

  case 65:
#line 432 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1990 "yacc_sql.tab.c"
    break;

  case 66:
#line 442 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-6].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;   //此字段在查询时未用到
			CONTEXT->select_length=0;  //此字段在查询时未用到
			CONTEXT->value_length = 0;
	}
#line 2010 "yacc_sql.tab.c"
    break;

  case 67:
#line 461 "yacc_sql.y"
    {
        //存在子查询
        CONTEXT->ssql->exisit_sub_select = 1;
        //子查询做完了，切换回主查询
        //交换子查询和主查询的queries
        swap_queries(&CONTEXT->ssql->sstr, CONTEXT->ssql->sub_sstr);
        //交换ParserContext中暂存condition的数组
        swap_conditions(CONTEXT->conditions, CONTEXT->sub_conditions, MAX_NUM);
        //交换condition_length和value_length
        swap_number(&CONTEXT->condition_length, &CONTEXT->sub_condition_length);
        swap_number(&CONTEXT->value_length, &CONTEXT->sub_value_length);

        swap_compOp(&CONTEXT->comp, &CONTEXT->sub_comp);

        //和子查询相关的临时变量清0
        CONTEXT->sub_value_length = 0;
        CONTEXT->sub_condition_length = 0;

        //返回SimpleSubSelect
        (yyval.s_select) = (SimpleSubSelect*)malloc(sizeof(SimpleSubSelect));
        (yyval.s_select)->sub_select_result = NULL;
    }
#line 2037 "yacc_sql.tab.c"
    break;

  case 68:
#line 487 "yacc_sql.y"
    {
        //切换到子查询
        CONTEXT->ssql->sub_sstr = (union Queries*)malloc(sizeof(union Queries));
        //交换子查询和主查询的queries
        swap_queries(&CONTEXT->ssql->sstr, CONTEXT->ssql->sub_sstr);
        //交换ParserContext中暂存condition的数组
        swap_conditions(CONTEXT->conditions, CONTEXT->sub_conditions, MAX_NUM);
        //交换condition_length和value_length
        swap_number(&CONTEXT->condition_length, &CONTEXT->sub_condition_length);
        swap_number(&CONTEXT->value_length, &CONTEXT->sub_value_length);

        swap_compOp(&CONTEXT->comp, &CONTEXT->sub_comp);
    }
#line 2055 "yacc_sql.tab.c"
    break;

  case 69:
#line 504 "yacc_sql.y"
    {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-6].string));

		selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

    }
#line 2066 "yacc_sql.tab.c"
    break;

  case 71:
#line 514 "yacc_sql.y"
                                        {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), NULL, (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }
#line 2076 "yacc_sql.tab.c"
    break;

  case 72:
#line 519 "yacc_sql.y"
                                               {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), (yyvsp[-4].string), (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }
#line 2086 "yacc_sql.tab.c"
    break;

  case 74:
#line 527 "yacc_sql.y"
                                {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2096 "yacc_sql.tab.c"
    break;

  case 75:
#line 532 "yacc_sql.y"
                                       {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2106 "yacc_sql.tab.c"
    break;

  case 77:
#line 541 "yacc_sql.y"
                                 {
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2116 "yacc_sql.tab.c"
    break;

  case 78:
#line 546 "yacc_sql.y"
                                        {
		GroupAttr group_attr;
		group_attr_init(&group_attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 2126 "yacc_sql.tab.c"
    break;

  case 80:
#line 555 "yacc_sql.y"
                                     {
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), NULL, (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
#line 2136 "yacc_sql.tab.c"
    break;

  case 81:
#line 560 "yacc_sql.y"
                                            {
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), (yyvsp[-4].string), (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
#line 2146 "yacc_sql.tab.c"
    break;

  case 82:
#line 568 "yacc_sql.y"
                {
        (yyval.number) = ASC_T; // 默认升序
    }
#line 2154 "yacc_sql.tab.c"
    break;

  case 83:
#line 571 "yacc_sql.y"
          {
        (yyval.number) = ASC_T; //上一行的ASC表示token 这一行的ASC表示的是enum里的类型
    }
#line 2162 "yacc_sql.tab.c"
    break;

  case 84:
#line 574 "yacc_sql.y"
           {
        (yyval.number) = DESC_T; // 
    }
#line 2170 "yacc_sql.tab.c"
    break;

  case 85:
#line 580 "yacc_sql.y"
         {  
        RelAttr attr;
        relation_attr_init(&attr, NULL, "*", NULL);
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2180 "yacc_sql.tab.c"
    break;

  case 86:
#line 586 "yacc_sql.y"
                       {

    }
#line 2188 "yacc_sql.tab.c"
    break;

  case 87:
#line 592 "yacc_sql.y"
                       {
        RelAttr attr;
        relation_attr_init(&attr, NULL, NULL, (yyvsp[0].exp_node));
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
    }
#line 2198 "yacc_sql.tab.c"
    break;

  case 88:
#line 597 "yacc_sql.y"
           {

    }
#line 2206 "yacc_sql.tab.c"
    break;

  case 90:
#line 604 "yacc_sql.y"
                           {

    }
#line 2214 "yacc_sql.tab.c"
    break;

  case 91:
#line 611 "yacc_sql.y"
                                                     {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init((yyval.exp_node), 1, (yyvsp[-2].exp_node), (yyvsp[-1].number), (yyvsp[0].exp_node), 0, NULL, NULL, 0, ADD);
    }
#line 2223 "yacc_sql.tab.c"
    break;

  case 92:
#line 615 "yacc_sql.y"
                         {
        (yyval.exp_node) = (yyvsp[0].exp_node);
    }
#line 2231 "yacc_sql.tab.c"
    break;

  case 93:
#line 621 "yacc_sql.y"
           {
        (yyval.number) = ADD;
    }
#line 2239 "yacc_sql.tab.c"
    break;

  case 94:
#line 624 "yacc_sql.y"
             {
        (yyval.number) = SUB;
    }
#line 2247 "yacc_sql.tab.c"
    break;

  case 95:
#line 630 "yacc_sql.y"
                                                  {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init((yyval.exp_node), 1, (yyvsp[-2].exp_node), (yyvsp[-1].number), (yyvsp[0].exp_node), 0, NULL, NULL, 0, ADD);
    }
#line 2256 "yacc_sql.tab.c"
    break;

  case 96:
#line 634 "yacc_sql.y"
                      {
        (yyval.exp_node) = (yyvsp[0].exp_node);
    }
#line 2264 "yacc_sql.tab.c"
    break;

  case 97:
#line 640 "yacc_sql.y"
         {
        /* 记录操作符 */
        (yyval.number) = MUL;
    }
#line 2273 "yacc_sql.tab.c"
    break;

  case 98:
#line 644 "yacc_sql.y"
             {
        /* 记录操作符 */
        (yyval.number) = DIV;
    }
#line 2282 "yacc_sql.tab.c"
    break;

  case 99:
#line 651 "yacc_sql.y"
                                           {
        (yyval.exp_node) = (yyvsp[-1].exp_node);
        (yyval.exp_node)->isBracket = 1;
        (yyval.exp_node)->sign = SUB;
    }
#line 2292 "yacc_sql.tab.c"
    break;

  case 100:
#line 656 "yacc_sql.y"
                                     {
        (yyval.exp_node) = (yyvsp[-1].exp_node);
        (yyval.exp_node)->isBracket = 1;
    }
#line 2301 "yacc_sql.tab.c"
    break;

  case 101:
#line 661 "yacc_sql.y"
           {
        /* 记录 列名 */
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, NULL, (yyvsp[0].string), NULL);
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 0, attr, NULL, 0, ADD);
	}
#line 2313 "yacc_sql.tab.c"
    break;

  case 102:
#line 669 "yacc_sql.y"
              {
        /* 记录表名 */
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string), NULL);
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 0, attr, NULL, 0, ADD );
	}
#line 2325 "yacc_sql.tab.c"
    break;

  case 103:
#line 676 "yacc_sql.y"
            {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        Value *value_ = &CONTEXT->values[CONTEXT->value_length - 1];
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 1, NULL, value_, 0, ADD);
    }
#line 2335 "yacc_sql.tab.c"
    break;

  case 104:
#line 684 "yacc_sql.y"
                                   {
            ExpressionNode node;
            RelAttr rel_attr;
            relation_attr_init(&rel_attr,NULL,"*",NULL);
            expression_node_init(&node,0,NULL,0,NULL,0,&rel_attr,NULL,0 , ADD);
			AggrAttr attr;
			/*第二个参数refer to AggrType */
			aggr_attr_init(&attr, (yyvsp[-3].number), &node);
			selects_append_aggr_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2350 "yacc_sql.tab.c"
    break;

  case 105:
#line 694 "yacc_sql.y"
                                               {
            ExpressionNode *node = (yyvsp[-1].exp_node);
            AggrAttr attr;
			/*第二个参数refer to AggrType */
			aggr_attr_init(&attr, (yyvsp[-3].number), node);
			selects_append_aggr_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2362 "yacc_sql.tab.c"
    break;

  case 106:
#line 704 "yacc_sql.y"
                   { (yyval.number)=1; }
#line 2368 "yacc_sql.tab.c"
    break;

  case 107:
#line 705 "yacc_sql.y"
                    { (yyval.number)=2; }
#line 2374 "yacc_sql.tab.c"
    break;

  case 108:
#line 706 "yacc_sql.y"
                   { (yyval.number)=3; }
#line 2380 "yacc_sql.tab.c"
    break;

  case 109:
#line 707 "yacc_sql.y"
                   { (yyval.number)=4; }
#line 2386 "yacc_sql.tab.c"
    break;

  case 111:
#line 713 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2394 "yacc_sql.tab.c"
    break;

  case 113:
#line 719 "yacc_sql.y"
                                               {
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
		}
#line 2402 "yacc_sql.tab.c"
    break;

  case 115:
#line 725 "yacc_sql.y"
                                      {

		}
#line 2410 "yacc_sql.tab.c"
    break;

  case 117:
#line 733 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2418 "yacc_sql.tab.c"
    break;

  case 119:
#line 739 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2426 "yacc_sql.tab.c"
    break;

  case 120:
#line 744 "yacc_sql.y"
                                                {
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, (yyvsp[-2].exp_node), (yyvsp[0].exp_node), NULL, 0);
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
#line 2436 "yacc_sql.tab.c"
    break;

  case 121:
#line 749 "yacc_sql.y"
                                          {
        //存在简单子查询，创建带简单子查询的condition
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, (yyvsp[-2].exp_node), NULL, (yyvsp[0].s_select), 1);
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
#line 2447 "yacc_sql.tab.c"
    break;

  case 122:
#line 755 "yacc_sql.y"
                                          {
        //存在简单子查询，创建带简单子查询的condition
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, NULL, (yyvsp[0].exp_node), (yyvsp[-2].s_select), 0);
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
#line 2458 "yacc_sql.tab.c"
    break;

  case 123:
#line 764 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2464 "yacc_sql.tab.c"
    break;

  case 124:
#line 765 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2470 "yacc_sql.tab.c"
    break;

  case 125:
#line 766 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2476 "yacc_sql.tab.c"
    break;

  case 126:
#line 767 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2482 "yacc_sql.tab.c"
    break;

  case 127:
#line 768 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2488 "yacc_sql.tab.c"
    break;

  case 128:
#line 769 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2494 "yacc_sql.tab.c"
    break;

  case 129:
#line 770 "yacc_sql.y"
                  { CONTEXT->comp = IS; }
#line 2500 "yacc_sql.tab.c"
    break;

  case 130:
#line 771 "yacc_sql.y"
                      { CONTEXT->comp = ISNOT; }
#line 2506 "yacc_sql.tab.c"
    break;

  case 131:
#line 772 "yacc_sql.y"
         { CONTEXT->comp = IS_IN; }
#line 2512 "yacc_sql.tab.c"
    break;

  case 132:
#line 773 "yacc_sql.y"
             { CONTEXT->comp = NOT_IN; }
#line 2518 "yacc_sql.tab.c"
    break;

  case 133:
#line 778 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2527 "yacc_sql.tab.c"
    break;


#line 2531 "yacc_sql.tab.c"

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
#line 783 "yacc_sql.y"

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
