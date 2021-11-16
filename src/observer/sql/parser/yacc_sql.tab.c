/* A Bison parser, made by GNU Bison 3.7.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7"

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


#line 136 "yacc_sql.tab.c"

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

#include "yacc_sql.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_INDEX = 8,                      /* INDEX  */
  YYSYMBOL_SELECT = 9,                     /* SELECT  */
  YYSYMBOL_DESC = 10,                      /* DESC  */
  YYSYMBOL_ASC = 11,                       /* ASC  */
  YYSYMBOL_SHOW = 12,                      /* SHOW  */
  YYSYMBOL_SYNC = 13,                      /* SYNC  */
  YYSYMBOL_INSERT = 14,                    /* INSERT  */
  YYSYMBOL_DELETE = 15,                    /* DELETE  */
  YYSYMBOL_UPDATE = 16,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 17,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 18,                    /* RBRACE  */
  YYSYMBOL_COMMA = 19,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 20,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 21,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 22,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 23,                     /* INT_T  */
  YYSYMBOL_STRING_T = 24,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 25,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 26,                    /* DATE_T  */
  YYSYMBOL_TEXT_T = 27,                    /* TEXT_T  */
  YYSYMBOL_HELP = 28,                      /* HELP  */
  YYSYMBOL_EXIT = 29,                      /* EXIT  */
  YYSYMBOL_DOT = 30,                       /* DOT  */
  YYSYMBOL_INTO = 31,                      /* INTO  */
  YYSYMBOL_VALUES = 32,                    /* VALUES  */
  YYSYMBOL_FROM = 33,                      /* FROM  */
  YYSYMBOL_WHERE = 34,                     /* WHERE  */
  YYSYMBOL_AND = 35,                       /* AND  */
  YYSYMBOL_SET = 36,                       /* SET  */
  YYSYMBOL_ON = 37,                        /* ON  */
  YYSYMBOL_LOAD = 38,                      /* LOAD  */
  YYSYMBOL_DATA = 39,                      /* DATA  */
  YYSYMBOL_INFILE = 40,                    /* INFILE  */
  YYSYMBOL_EQ = 41,                        /* EQ  */
  YYSYMBOL_LT = 42,                        /* LT  */
  YYSYMBOL_GT = 43,                        /* GT  */
  YYSYMBOL_LE = 44,                        /* LE  */
  YYSYMBOL_GE = 45,                        /* GE  */
  YYSYMBOL_NE = 46,                        /* NE  */
  YYSYMBOL_AGGR_COUNT = 47,                /* AGGR_COUNT  */
  YYSYMBOL_AGGR_MAX = 48,                  /* AGGR_MAX  */
  YYSYMBOL_AGGR_MIN = 49,                  /* AGGR_MIN  */
  YYSYMBOL_AGGR_AVG = 50,                  /* AGGR_AVG  */
  YYSYMBOL_UNIQUE = 51,                    /* UNIQUE  */
  YYSYMBOL_NOT = 52,                       /* NOT  */
  YYSYMBOL_NULLTOKEN = 53,                 /* NULLTOKEN  */
  YYSYMBOL_NULLABLE = 54,                  /* NULLABLE  */
  YYSYMBOL_ISTOKEN = 55,                   /* ISTOKEN  */
  YYSYMBOL_ORDER = 56,                     /* ORDER  */
  YYSYMBOL_GROUP = 57,                     /* GROUP  */
  YYSYMBOL_BY = 58,                        /* BY  */
  YYSYMBOL_INNER = 59,                     /* INNER  */
  YYSYMBOL_JOIN = 60,                      /* JOIN  */
  YYSYMBOL_ADD_OP = 61,                    /* ADD_OP  */
  YYSYMBOL_SUB_OP = 62,                    /* SUB_OP  */
  YYSYMBOL_DIV_OP = 63,                    /* DIV_OP  */
  YYSYMBOL_IN = 64,                        /* IN  */
  YYSYMBOL_NUMBER = 65,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 66,                     /* FLOAT  */
  YYSYMBOL_ID = 67,                        /* ID  */
  YYSYMBOL_DATE = 68,                      /* DATE  */
  YYSYMBOL_PATH = 69,                      /* PATH  */
  YYSYMBOL_SSS = 70,                       /* SSS  */
  YYSYMBOL_STAR = 71,                      /* STAR  */
  YYSYMBOL_STRING_V = 72,                  /* STRING_V  */
  YYSYMBOL_YYACCEPT = 73,                  /* $accept  */
  YYSYMBOL_commands = 74,                  /* commands  */
  YYSYMBOL_command = 75,                   /* command  */
  YYSYMBOL_exit = 76,                      /* exit  */
  YYSYMBOL_help = 77,                      /* help  */
  YYSYMBOL_sync = 78,                      /* sync  */
  YYSYMBOL_begin = 79,                     /* begin  */
  YYSYMBOL_commit = 80,                    /* commit  */
  YYSYMBOL_rollback = 81,                  /* rollback  */
  YYSYMBOL_drop_table = 82,                /* drop_table  */
  YYSYMBOL_show_tables = 83,               /* show_tables  */
  YYSYMBOL_desc_table = 84,                /* desc_table  */
  YYSYMBOL_create_index = 85,              /* create_index  */
  YYSYMBOL_index_type = 86,                /* index_type  */
  YYSYMBOL_id_list = 87,                   /* id_list  */
  YYSYMBOL_index_attr_id = 88,             /* index_attr_id  */
  YYSYMBOL_drop_index = 89,                /* drop_index  */
  YYSYMBOL_create_table = 90,              /* create_table  */
  YYSYMBOL_attr_def_list = 91,             /* attr_def_list  */
  YYSYMBOL_attr_def = 92,                  /* attr_def  */
  YYSYMBOL_number = 93,                    /* number  */
  YYSYMBOL_type = 94,                      /* type  */
  YYSYMBOL_ID_get = 95,                    /* ID_get  */
  YYSYMBOL_insert = 96,                    /* insert  */
  YYSYMBOL_record_list = 97,               /* record_list  */
  YYSYMBOL_record = 98,                    /* record  */
  YYSYMBOL_value_list = 99,                /* value_list  */
  YYSYMBOL_value = 100,                    /* value  */
  YYSYMBOL_sign = 101,                     /* sign  */
  YYSYMBOL_delete = 102,                   /* delete  */
  YYSYMBOL_update = 103,                   /* update  */
  YYSYMBOL_select = 104,                   /* select  */
  YYSYMBOL_sub_select = 105,               /* sub_select  */
  YYSYMBOL_sub_select_flag = 106,          /* sub_select_flag  */
  YYSYMBOL_sub_select_action = 107,        /* sub_select_action  */
  YYSYMBOL_order = 108,                    /* order  */
  YYSYMBOL_group = 109,                    /* group  */
  YYSYMBOL_group_id_list = 110,            /* group_id_list  */
  YYSYMBOL_order_list = 111,               /* order_list  */
  YYSYMBOL_order_type = 112,               /* order_type  */
  YYSYMBOL_select_attr = 113,              /* select_attr  */
  YYSYMBOL_attr = 114,                     /* attr  */
  YYSYMBOL_attr_list = 115,                /* attr_list  */
  YYSYMBOL_add_sub_expression = 116,       /* add_sub_expression  */
  YYSYMBOL_add_or_sub = 117,               /* add_or_sub  */
  YYSYMBOL_mul_div_expression = 118,       /* mul_div_expression  */
  YYSYMBOL_mul_or_div = 119,               /* mul_or_div  */
  YYSYMBOL_atom_expression = 120,          /* atom_expression  */
  YYSYMBOL_aggr = 121,                     /* aggr  */
  YYSYMBOL_aggr_op = 122,                  /* aggr_op  */
  YYSYMBOL_rel_list = 123,                 /* rel_list  */
  YYSYMBOL_inner_join = 124,               /* inner_join  */
  YYSYMBOL_on = 125,                       /* on  */
  YYSYMBOL_where = 126,                    /* where  */
  YYSYMBOL_condition_list = 127,           /* condition_list  */
  YYSYMBOL_condition = 128,                /* condition  */
  YYSYMBOL_comOp = 129,                    /* comOp  */
  YYSYMBOL_load_data = 130                 /* load_data  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

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
#define YYLAST   278

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  259

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   327


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

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
     402,   406,   412,   415,   421,   431,   441,   460,   486,   502,
     511,   513,   518,   524,   526,   531,   538,   540,   545,   552,
     554,   559,   567,   570,   573,   579,   585,   591,   596,   601,
     603,   610,   614,   620,   623,   629,   633,   639,   643,   650,
     656,   664,   671,   679,   689,   699,   700,   701,   702,   706,
     708,   712,   714,   718,   720,   726,   728,   732,   734,   739,
     744,   750,   759,   760,   761,   762,   763,   764,   765,   766,
     767,   768,   772
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "INDEX", "SELECT", "DESC", "ASC", "SHOW",
  "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA",
  "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T",
  "FLOAT_T", "DATE_T", "TEXT_T", "HELP", "EXIT", "DOT", "INTO", "VALUES",
  "FROM", "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ",
  "LT", "GT", "LE", "GE", "NE", "AGGR_COUNT", "AGGR_MAX", "AGGR_MIN",
  "AGGR_AVG", "UNIQUE", "NOT", "NULLTOKEN", "NULLABLE", "ISTOKEN", "ORDER",
  "GROUP", "BY", "INNER", "JOIN", "ADD_OP", "SUB_OP", "DIV_OP", "IN",
  "NUMBER", "FLOAT", "ID", "DATE", "PATH", "SSS", "STAR", "STRING_V",
  "$accept", "commands", "command", "exit", "help", "sync", "begin",
  "commit", "rollback", "drop_table", "show_tables", "desc_table",
  "create_index", "index_type", "id_list", "index_attr_id", "drop_index",
  "create_table", "attr_def_list", "attr_def", "number", "type", "ID_get",
  "insert", "record_list", "record", "value_list", "value", "sign",
  "delete", "update", "select", "sub_select", "sub_select_flag",
  "sub_select_action", "order", "group", "group_id_list", "order_list",
  "order_type", "select_attr", "attr", "attr_list", "add_sub_expression",
  "add_or_sub", "mul_div_expression", "mul_or_div", "atom_expression",
  "aggr", "aggr_op", "rel_list", "inner_join", "on", "where",
  "condition_list", "condition", "comOp", "load_data", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
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
#endif

#define YYPACT_NINF (-233)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -233,    82,  -233,     1,    23,   -28,   -26,    67,    86,    59,
      72,    39,   105,   116,   118,   119,   121,    68,  -233,  -233,
    -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,
    -233,  -233,  -233,  -233,  -233,  -233,    42,  -233,   110,    58,
      61,  -233,  -233,  -233,  -233,  -233,  -233,  -233,    99,  -233,
    -233,  -233,  -233,   -12,    98,   117,   -17,  -233,    30,  -233,
    -233,   120,   131,   140,  -233,    77,    79,   127,  -233,  -233,
    -233,  -233,  -233,   124,   138,    89,   164,   166,   112,   115,
    -233,  -233,   113,    92,  -233,   115,  -233,  -233,   115,   104,
    -233,  -233,   141,   147,   129,   114,   130,   157,  -233,  -233,
    -233,     8,   179,   117,    30,  -233,   181,   182,   184,    -2,
     199,   162,   173,  -233,   186,   165,   139,  -233,   142,   148,
    -233,  -233,  -233,   125,   189,   201,   106,    92,    71,   176,
    -233,   125,   206,   130,   195,  -233,  -233,  -233,  -233,  -233,
      -5,   197,   179,   155,   147,   198,     6,   184,   213,  -233,
    -233,  -233,  -233,  -233,  -233,  -233,   154,   167,  -233,   115,
    -233,   187,    -2,    -2,  -233,   147,   156,   186,   218,   159,
     169,  -233,   158,  -233,   160,   171,   125,   208,   189,  -233,
    -233,  -233,   -17,   163,  -233,   -17,   176,   226,   228,  -233,
    -233,  -233,   214,  -233,  -233,   215,   179,   175,   180,   198,
    -233,  -233,   179,  -233,  -233,  -233,  -233,   158,   217,   200,
     172,   183,   235,  -233,   148,   215,   237,    -2,   148,    45,
     177,  -233,   147,  -233,  -233,   176,  -233,   178,   185,  -233,
      25,   171,  -233,    48,   223,  -233,  -233,   188,   224,   180,
     190,  -233,  -233,    66,   191,  -233,   229,   223,   224,    27,
    -233,  -233,  -233,   192,   224,    66,  -233,   224,  -233
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
       0,   105,   107,   108,   106,    61,    93,    94,   100,    60,
      59,    85,   102,     0,     0,    89,    87,    63,    92,    96,
      88,     0,     0,     0,    23,     0,     0,     0,    24,    25,
      26,    22,    21,     0,     0,     0,     0,     0,     0,    62,
      57,    58,     0,    62,    86,    62,    98,    97,    62,    62,
      29,    28,     0,   115,     0,     0,     0,     0,    27,    36,
     101,     0,   109,    89,    91,    95,     0,     0,     0,    62,
       0,     0,     0,    50,    38,     0,     0,    99,     0,   111,
      90,   103,   104,    62,    52,     0,     0,    62,     0,   117,
      64,    62,     0,     0,     0,    45,    46,    47,    48,    49,
      41,     0,   109,     0,   115,    55,     0,     0,     0,    68,
     122,   123,   124,   125,   126,   127,     0,   128,   130,    62,
      67,     0,    62,    62,   116,   115,     0,    38,     0,     0,
       0,    43,     0,   110,     0,    73,    62,     0,    52,    51,
     131,   129,   121,     0,   120,   119,   117,     0,     0,    39,
      37,    44,     0,    42,    35,    33,   109,     0,    70,    55,
      54,    53,   109,   118,    65,   132,    40,     0,     0,   113,
       0,     0,     0,    56,   111,    33,     0,    62,   111,    76,
       0,    66,   115,    34,    30,   117,   112,     0,     0,    74,
      82,    73,   114,    76,    76,    84,    83,     0,    79,    70,
       0,    77,    75,    82,     0,    71,     0,    76,    79,    82,
      69,    78,    72,     0,    79,    82,    80,    79,    81
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,
    -233,  -233,  -233,  -233,    31,    41,  -233,  -233,    83,   123,
    -233,  -233,  -233,  -233,    73,   102,    54,  -115,  -113,  -233,
    -233,  -233,   100,  -233,  -233,    15,    29,  -220,  -169,  -232,
    -233,   -77,   161,   -79,   -55,   193,  -233,    11,  -233,  -233,
    -140,  -186,  -233,  -141,  -177,  -159,   133,  -233
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    38,   208,   195,    29,    30,   134,   114,
     192,   140,   115,    31,   148,   124,   177,    52,    53,    32,
      33,    34,   126,   127,   160,   212,   198,   229,   245,   238,
      54,    55,    84,    56,    57,    58,    88,    59,    60,    61,
     119,   144,   218,   110,   164,   129,   159,    35
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     101,    85,   173,   175,   186,    79,   103,    36,   145,   203,
     146,   248,   169,   241,   242,   125,   165,   254,   146,    41,
      42,    43,    44,   257,   187,    45,   117,   251,   222,    39,
     128,    40,   226,    46,    47,   235,   236,   235,   236,    48,
      49,    62,    50,    51,    46,    47,    85,   170,   232,   171,
     161,    45,    37,    80,    81,   237,   209,   253,   225,    46,
      47,   199,   214,   146,   227,    48,    49,   227,    50,    46,
      47,    80,    81,    85,    63,   228,   235,   236,   240,   252,
     182,   231,     2,   185,   128,   256,     3,     4,   258,    64,
      65,     5,     6,    86,     7,     8,     9,    10,    11,   105,
     107,    87,    12,    13,    14,    66,    67,    73,    68,    74,
      15,    16,   150,   151,   152,   153,   154,   155,    75,    69,
      17,    70,    71,   156,    72,    76,   157,    85,    77,    78,
      85,    82,    46,    47,    90,   158,    83,    89,   128,    41,
      42,    43,    44,    91,    92,    45,    93,   150,   151,   152,
     153,   154,   155,    46,    47,    96,    97,    45,   156,    48,
      49,   157,    50,    94,    95,    46,    47,    98,    45,    99,
     158,    48,    49,   108,    50,   106,    46,    47,    45,   100,
     102,   109,    48,    49,   112,    50,    46,    47,   135,   136,
     137,   138,   139,    49,   116,    50,   111,   113,   118,   121,
     122,   123,   130,   131,   132,   133,   141,   143,   147,   142,
     149,   163,   166,   168,   172,   174,   179,   176,   180,   181,
     183,   190,   193,   188,   191,   194,   200,   196,   197,   204,
     202,   205,   206,   210,   207,   216,   211,   217,   221,   219,
     224,   220,   227,   244,   230,   233,   223,   250,   215,   178,
     189,   201,   234,   213,   246,   243,   167,   247,   249,   255,
     239,   162,   184,     0,   120,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   104
};

static const yytype_int16 yycheck[] =
{
      79,    56,   142,   144,   163,    17,    83,     6,   123,   186,
     123,   243,    17,   233,   234,    17,   131,   249,   131,    47,
      48,    49,    50,   255,   165,    53,    18,   247,   214,     6,
     109,     8,   218,    61,    62,    10,    11,    10,    11,    67,
      68,    67,    70,    71,    61,    62,   101,    52,   225,    54,
     127,    53,    51,    65,    66,    30,   196,    30,   217,    61,
      62,   176,   202,   176,    19,    67,    68,    19,    70,    61,
      62,    65,    66,   128,     7,    30,    10,    11,    30,   248,
     159,   222,     0,   162,   163,   254,     4,     5,   257,     3,
      31,     9,    10,    63,    12,    13,    14,    15,    16,    88,
      89,    71,    20,    21,    22,    33,    67,    39,     3,    67,
      28,    29,    41,    42,    43,    44,    45,    46,     8,     3,
      38,     3,     3,    52,     3,    67,    55,   182,    67,    30,
     185,    33,    61,    62,     3,    64,    19,    17,   217,    47,
      48,    49,    50,     3,    67,    53,    67,    41,    42,    43,
      44,    45,    46,    61,    62,    17,    67,    53,    52,    67,
      68,    55,    70,    36,    40,    61,    62,     3,    53,     3,
      64,    67,    68,    32,    70,    71,    61,    62,    53,    67,
      67,    34,    67,    68,    70,    70,    61,    62,    23,    24,
      25,    26,    27,    68,    37,    70,    67,    67,    19,    18,
      18,    17,     3,    41,    31,    19,    67,    59,    19,    67,
       9,    35,     6,    18,    17,    60,     3,    19,    64,    52,
      33,     3,    53,    67,    65,    67,    18,    67,    57,     3,
      67,     3,    18,    58,    19,    18,    56,    37,     3,    67,
       3,    58,    19,    19,    67,    67,   215,    18,   207,   147,
     167,   178,    67,   199,   239,    67,   133,    67,    67,    67,
     231,   128,   162,    -1,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,     0,     4,     5,     9,    10,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    38,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    89,
      90,    96,   102,   103,   104,   130,     6,    51,    86,     6,
       8,    47,    48,    49,    50,    53,    61,    62,    67,    68,
      70,    71,   100,   101,   113,   114,   116,   117,   118,   120,
     121,   122,    67,     7,     3,    31,    33,    67,     3,     3,
       3,     3,     3,    39,    67,     8,    67,    67,    30,    17,
      65,    66,    33,    19,   115,   117,    63,    71,   119,    17,
       3,     3,    67,    67,    36,    40,    17,    67,     3,     3,
      67,   116,    67,   114,   118,   120,    71,   120,    32,    34,
     126,    67,    70,    67,    92,    95,    37,    18,    19,   123,
     115,    18,    18,    17,    98,    17,   105,   106,   116,   128,
       3,    41,    31,    19,    91,    23,    24,    25,    26,    27,
      94,    67,    67,    59,   124,   100,   101,    19,    97,     9,
      41,    42,    43,    44,    45,    46,    52,    55,    64,   129,
     107,   114,   129,    35,   127,   100,     6,    92,    18,    17,
      52,    54,    17,   123,    60,   126,    19,    99,    98,     3,
      64,    52,   116,    33,   105,   116,   128,   126,    67,    91,
       3,    65,    93,    53,    67,    88,    67,    57,   109,   100,
      18,    97,    67,   127,     3,     3,    18,    19,    87,   123,
      58,    56,   108,    99,   123,    88,    18,    37,   125,    67,
      58,     3,   124,    87,     3,   128,   124,    19,    30,   110,
      67,   126,   127,    67,    67,    10,    11,    30,   112,   109,
      30,   110,   110,    67,    19,   111,   108,    67,   112,    67,
      18,   110,   111,    30,   112,    67,   111,   112,   111
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
     120,   120,   120,   121,   121,   122,   122,   122,   122,   123,
     123,   124,   124,   125,   125,   126,   126,   127,   127,   128,
     128,   128,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   130
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
       1,     3,     1,     4,     4,     1,     1,     1,     1,     0,
       3,     0,     6,     0,     3,     0,     3,     0,     3,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, void *scanner)
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
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

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
  YY_STACK_PRINT (yyss, yyssp);

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
#  undef YYSTACK_RELOCATE
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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  case 21: /* exit: EXIT SEMICOLON  */
#line 202 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1455 "yacc_sql.tab.c"
    break;

  case 22: /* help: HELP SEMICOLON  */
#line 207 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1463 "yacc_sql.tab.c"
    break;

  case 23: /* sync: SYNC SEMICOLON  */
#line 212 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1471 "yacc_sql.tab.c"
    break;

  case 24: /* begin: TRX_BEGIN SEMICOLON  */
#line 218 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1479 "yacc_sql.tab.c"
    break;

  case 25: /* commit: TRX_COMMIT SEMICOLON  */
#line 224 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1487 "yacc_sql.tab.c"
    break;

  case 26: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 230 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1495 "yacc_sql.tab.c"
    break;

  case 27: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 236 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1504 "yacc_sql.tab.c"
    break;

  case 28: /* show_tables: SHOW TABLES SEMICOLON  */
#line 242 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1512 "yacc_sql.tab.c"
    break;

  case 29: /* desc_table: DESC ID SEMICOLON  */
#line 248 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1521 "yacc_sql.tab.c"
    break;

  case 30: /* create_index: CREATE index_type INDEX ID ON ID LBRACE index_attr_id id_list RBRACE SEMICOLON  */
#line 256 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1530 "yacc_sql.tab.c"
    break;

  case 32: /* index_type: UNIQUE  */
#line 264 "yacc_sql.y"
             {
        create_index_unique_init(&CONTEXT->ssql->sstr.create_index);
    }
#line 1538 "yacc_sql.tab.c"
    break;

  case 35: /* index_attr_id: ID  */
#line 273 "yacc_sql.y"
       {
        create_index_attr_init(&CONTEXT->ssql->sstr.create_index,(yyvsp[0].string));
    }
#line 1546 "yacc_sql.tab.c"
    break;

  case 36: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 280 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1555 "yacc_sql.tab.c"
    break;

  case 37: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 287 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1567 "yacc_sql.tab.c"
    break;

  case 39: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 297 "yacc_sql.y"
                                   {    }
#line 1573 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def: ID_get type LBRACE number RBRACE  */
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
#line 1588 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type  */
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
#line 1603 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type NOT NULLTOKEN  */
#line 323 "yacc_sql.y"
                                   {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-2].number), 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
#line 1614 "yacc_sql.tab.c"
    break;

  case 43: /* attr_def: ID_get type NULLABLE  */
#line 329 "yacc_sql.y"
                              {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
#line 1625 "yacc_sql.tab.c"
    break;

  case 44: /* number: NUMBER  */
#line 337 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1631 "yacc_sql.tab.c"
    break;

  case 45: /* type: INT_T  */
#line 340 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1637 "yacc_sql.tab.c"
    break;

  case 46: /* type: STRING_T  */
#line 341 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1643 "yacc_sql.tab.c"
    break;

  case 47: /* type: FLOAT_T  */
#line 342 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1649 "yacc_sql.tab.c"
    break;

  case 48: /* type: DATE_T  */
#line 344 "yacc_sql.y"
                { (yyval.number)=DATES; }
#line 1655 "yacc_sql.tab.c"
    break;

  case 49: /* type: TEXT_T  */
#line 345 "yacc_sql.y"
                {(yyval.number)=TEXTS;}
#line 1661 "yacc_sql.tab.c"
    break;

  case 50: /* ID_get: ID  */
#line 349 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1670 "yacc_sql.tab.c"
    break;

  case 51: /* insert: INSERT INTO ID VALUES record record_list SEMICOLON  */
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
#line 1690 "yacc_sql.tab.c"
    break;

  case 54: /* record: LBRACE value value_list RBRACE  */
#line 379 "yacc_sql.y"
                                   {
        record_init(&CONTEXT->records[CONTEXT->record_length++],CONTEXT->values,CONTEXT->value_length);
        CONTEXT->value_length = 0;
    }
#line 1699 "yacc_sql.tab.c"
    break;

  case 56: /* value_list: COMMA value value_list  */
#line 386 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1707 "yacc_sql.tab.c"
    break;

  case 57: /* value: sign NUMBER  */
#line 391 "yacc_sql.y"
               {
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number),(yyvsp[-1].number));
		}
#line 1715 "yacc_sql.tab.c"
    break;

  case 58: /* value: sign FLOAT  */
#line 394 "yacc_sql.y"
               {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats),(yyvsp[-1].number));
		}
#line 1723 "yacc_sql.tab.c"
    break;

  case 59: /* value: SSS  */
#line 397 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1732 "yacc_sql.tab.c"
    break;

  case 60: /* value: DATE  */
#line 402 "yacc_sql.y"
          {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1741 "yacc_sql.tab.c"
    break;

  case 61: /* value: NULLTOKEN  */
#line 406 "yacc_sql.y"
                   {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 1749 "yacc_sql.tab.c"
    break;

  case 62: /* sign: %empty  */
#line 412 "yacc_sql.y"
                {
        (yyval.number) = ADD;
    }
#line 1757 "yacc_sql.tab.c"
    break;

  case 63: /* sign: add_or_sub  */
#line 415 "yacc_sql.y"
                 {
        (yyval.number) = (yyvsp[0].number);
    }
#line 1765 "yacc_sql.tab.c"
    break;

  case 64: /* delete: DELETE FROM ID where SEMICOLON  */
#line 422 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1777 "yacc_sql.tab.c"
    break;

  case 65: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 432 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1789 "yacc_sql.tab.c"
    break;

  case 66: /* select: SELECT select_attr FROM ID rel_list inner_join where group order SEMICOLON  */
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
#line 1809 "yacc_sql.tab.c"
    break;

  case 67: /* sub_select: sub_select_flag sub_select_action  */
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
#line 1836 "yacc_sql.tab.c"
    break;

  case 68: /* sub_select_flag: LBRACE SELECT  */
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
#line 1854 "yacc_sql.tab.c"
    break;

  case 69: /* sub_select_action: attr FROM ID rel_list inner_join where group order RBRACE  */
#line 503 "yacc_sql.y"
    {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-6].string));

		selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

    }
#line 1865 "yacc_sql.tab.c"
    break;

  case 71: /* order: ORDER BY ID order_type order_list  */
#line 513 "yacc_sql.y"
                                        {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), NULL, (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }
#line 1875 "yacc_sql.tab.c"
    break;

  case 72: /* order: ORDER BY ID DOT ID order_type order_list  */
#line 518 "yacc_sql.y"
                                               {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), (yyvsp[-4].string), (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }
#line 1885 "yacc_sql.tab.c"
    break;

  case 74: /* group: GROUP BY ID group_id_list  */
#line 526 "yacc_sql.y"
                                {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 1895 "yacc_sql.tab.c"
    break;

  case 75: /* group: GROUP BY ID DOT ID group_id_list  */
#line 531 "yacc_sql.y"
                                       {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 1905 "yacc_sql.tab.c"
    break;

  case 77: /* group_id_list: COMMA ID group_id_list  */
#line 540 "yacc_sql.y"
                                 {
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 1915 "yacc_sql.tab.c"
    break;

  case 78: /* group_id_list: COMMA ID DOT ID group_id_list  */
#line 545 "yacc_sql.y"
                                        {
		GroupAttr group_attr;
		group_attr_init(&group_attr, (yyvsp[-3].string), (yyvsp[-1].string));
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
#line 1925 "yacc_sql.tab.c"
    break;

  case 80: /* order_list: COMMA ID order_type order_list  */
#line 554 "yacc_sql.y"
                                     {
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), NULL, (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
#line 1935 "yacc_sql.tab.c"
    break;

  case 81: /* order_list: COMMA ID DOT ID order_type order_list  */
#line 559 "yacc_sql.y"
                                            {
            OrderAttr order_attr;
            order_attr_init(&order_attr, (yyvsp[-1].number), (yyvsp[-4].string), (yyvsp[-2].string));
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
#line 1945 "yacc_sql.tab.c"
    break;

  case 82: /* order_type: %empty  */
#line 567 "yacc_sql.y"
                {
        (yyval.number) = ASC_T; // 默认升序
    }
#line 1953 "yacc_sql.tab.c"
    break;

  case 83: /* order_type: ASC  */
#line 570 "yacc_sql.y"
          {
        (yyval.number) = ASC_T; //上一行的ASC表示token 这一行的ASC表示的是enum里的类型
    }
#line 1961 "yacc_sql.tab.c"
    break;

  case 84: /* order_type: DESC  */
#line 573 "yacc_sql.y"
           {
        (yyval.number) = DESC_T; // 
    }
#line 1969 "yacc_sql.tab.c"
    break;

  case 85: /* select_attr: STAR  */
#line 579 "yacc_sql.y"
         {  
        RelAttr attr;
        relation_attr_init(&attr, NULL, "*", NULL);
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 1979 "yacc_sql.tab.c"
    break;

  case 86: /* select_attr: attr attr_list  */
#line 585 "yacc_sql.y"
                       {

    }
#line 1987 "yacc_sql.tab.c"
    break;

  case 87: /* attr: add_sub_expression  */
#line 591 "yacc_sql.y"
                       {
        RelAttr attr;
        relation_attr_init(&attr, NULL, NULL, (yyvsp[0].exp_node));
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
    }
#line 1997 "yacc_sql.tab.c"
    break;

  case 88: /* attr: aggr  */
#line 596 "yacc_sql.y"
           {

    }
#line 2005 "yacc_sql.tab.c"
    break;

  case 90: /* attr_list: COMMA attr attr_list  */
#line 603 "yacc_sql.y"
                           {

    }
#line 2013 "yacc_sql.tab.c"
    break;

  case 91: /* add_sub_expression: add_sub_expression add_or_sub mul_div_expression  */
#line 610 "yacc_sql.y"
                                                     {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init((yyval.exp_node), 1, (yyvsp[-2].exp_node), (yyvsp[-1].number), (yyvsp[0].exp_node), 0, NULL, NULL, 0, ADD);
    }
#line 2022 "yacc_sql.tab.c"
    break;

  case 92: /* add_sub_expression: mul_div_expression  */
#line 614 "yacc_sql.y"
                         {
        (yyval.exp_node) = (yyvsp[0].exp_node);
    }
#line 2030 "yacc_sql.tab.c"
    break;

  case 93: /* add_or_sub: ADD_OP  */
#line 620 "yacc_sql.y"
           {
        (yyval.number) = ADD;
    }
#line 2038 "yacc_sql.tab.c"
    break;

  case 94: /* add_or_sub: SUB_OP  */
#line 623 "yacc_sql.y"
             {
        (yyval.number) = SUB;
    }
#line 2046 "yacc_sql.tab.c"
    break;

  case 95: /* mul_div_expression: mul_div_expression mul_or_div atom_expression  */
#line 629 "yacc_sql.y"
                                                  {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init((yyval.exp_node), 1, (yyvsp[-2].exp_node), (yyvsp[-1].number), (yyvsp[0].exp_node), 0, NULL, NULL, 0, ADD);
    }
#line 2055 "yacc_sql.tab.c"
    break;

  case 96: /* mul_div_expression: atom_expression  */
#line 633 "yacc_sql.y"
                      {
        (yyval.exp_node) = (yyvsp[0].exp_node);
    }
#line 2063 "yacc_sql.tab.c"
    break;

  case 97: /* mul_or_div: STAR  */
#line 639 "yacc_sql.y"
         {
        /* 记录操作符 */
        (yyval.number) = MUL;
    }
#line 2072 "yacc_sql.tab.c"
    break;

  case 98: /* mul_or_div: DIV_OP  */
#line 643 "yacc_sql.y"
             {
        /* 记录操作符 */
        (yyval.number) = DIV;
    }
#line 2081 "yacc_sql.tab.c"
    break;

  case 99: /* atom_expression: sign LBRACE add_sub_expression RBRACE  */
#line 650 "yacc_sql.y"
                                         {
        (yyval.exp_node) = (yyvsp[-1].exp_node);
        (yyval.exp_node)->isBracket = 1;
        (yyval.exp_node)->sign = (yyvsp[-3].number);
    }
#line 2091 "yacc_sql.tab.c"
    break;

  case 100: /* atom_expression: ID  */
#line 656 "yacc_sql.y"
           {
        /* 记录 列名 */
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, NULL, (yyvsp[0].string), NULL);
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 0, attr, NULL, 0, ADD);
	}
#line 2103 "yacc_sql.tab.c"
    break;

  case 101: /* atom_expression: ID DOT ID  */
#line 664 "yacc_sql.y"
              {
        /* 记录表名 */
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, (yyvsp[-2].string), (yyvsp[0].string), NULL);
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 0, attr, NULL, 0, ADD );
	}
#line 2115 "yacc_sql.tab.c"
    break;

  case 102: /* atom_expression: value  */
#line 671 "yacc_sql.y"
            {
        (yyval.exp_node)=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        Value *value_ = &CONTEXT->values[CONTEXT->value_length - 1];
        expression_node_init((yyval.exp_node), 0, NULL, 0, NULL, 1, NULL, value_, 0, ADD);
    }
#line 2125 "yacc_sql.tab.c"
    break;

  case 103: /* aggr: aggr_op LBRACE STAR RBRACE  */
#line 679 "yacc_sql.y"
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
#line 2140 "yacc_sql.tab.c"
    break;

  case 104: /* aggr: aggr_op LBRACE atom_expression RBRACE  */
#line 689 "yacc_sql.y"
                                               {
            ExpressionNode *node = (yyvsp[-1].exp_node);
            AggrAttr attr;
			/*第二个参数refer to AggrType */
			aggr_attr_init(&attr, (yyvsp[-3].number), node);
			selects_append_aggr_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2152 "yacc_sql.tab.c"
    break;

  case 105: /* aggr_op: AGGR_COUNT  */
#line 699 "yacc_sql.y"
                   { (yyval.number)=1; }
#line 2158 "yacc_sql.tab.c"
    break;

  case 106: /* aggr_op: AGGR_AVG  */
#line 700 "yacc_sql.y"
                    { (yyval.number)=2; }
#line 2164 "yacc_sql.tab.c"
    break;

  case 107: /* aggr_op: AGGR_MAX  */
#line 701 "yacc_sql.y"
                   { (yyval.number)=3; }
#line 2170 "yacc_sql.tab.c"
    break;

  case 108: /* aggr_op: AGGR_MIN  */
#line 702 "yacc_sql.y"
                   { (yyval.number)=4; }
#line 2176 "yacc_sql.tab.c"
    break;

  case 110: /* rel_list: COMMA ID rel_list  */
#line 708 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2184 "yacc_sql.tab.c"
    break;

  case 112: /* inner_join: INNER JOIN ID rel_list on inner_join  */
#line 714 "yacc_sql.y"
                                               {
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
		}
#line 2192 "yacc_sql.tab.c"
    break;

  case 114: /* on: ON condition condition_list  */
#line 720 "yacc_sql.y"
                                      {

		}
#line 2200 "yacc_sql.tab.c"
    break;

  case 116: /* where: WHERE condition condition_list  */
#line 728 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2208 "yacc_sql.tab.c"
    break;

  case 118: /* condition_list: AND condition condition_list  */
#line 734 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2216 "yacc_sql.tab.c"
    break;

  case 119: /* condition: add_sub_expression comOp add_sub_expression  */
#line 739 "yacc_sql.y"
                                                {
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, (yyvsp[-2].exp_node), (yyvsp[0].exp_node), NULL, 0);
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
#line 2226 "yacc_sql.tab.c"
    break;

  case 120: /* condition: add_sub_expression comOp sub_select  */
#line 744 "yacc_sql.y"
                                          {
        //存在简单子查询，创建带简单子查询的condition
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, (yyvsp[-2].exp_node), NULL, (yyvsp[0].s_select), 1);
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
#line 2237 "yacc_sql.tab.c"
    break;

  case 121: /* condition: sub_select comOp add_sub_expression  */
#line 750 "yacc_sql.y"
                                          {
        //存在简单子查询，创建带简单子查询的condition
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, NULL, (yyvsp[0].exp_node), (yyvsp[-2].s_select), 0);
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
#line 2248 "yacc_sql.tab.c"
    break;

  case 122: /* comOp: EQ  */
#line 759 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2254 "yacc_sql.tab.c"
    break;

  case 123: /* comOp: LT  */
#line 760 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2260 "yacc_sql.tab.c"
    break;

  case 124: /* comOp: GT  */
#line 761 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2266 "yacc_sql.tab.c"
    break;

  case 125: /* comOp: LE  */
#line 762 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2272 "yacc_sql.tab.c"
    break;

  case 126: /* comOp: GE  */
#line 763 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2278 "yacc_sql.tab.c"
    break;

  case 127: /* comOp: NE  */
#line 764 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2284 "yacc_sql.tab.c"
    break;

  case 128: /* comOp: ISTOKEN  */
#line 765 "yacc_sql.y"
                  { CONTEXT->comp = IS; }
#line 2290 "yacc_sql.tab.c"
    break;

  case 129: /* comOp: ISTOKEN NOT  */
#line 766 "yacc_sql.y"
                      { CONTEXT->comp = ISNOT; }
#line 2296 "yacc_sql.tab.c"
    break;

  case 130: /* comOp: IN  */
#line 767 "yacc_sql.y"
         { CONTEXT->comp = IS_IN; }
#line 2302 "yacc_sql.tab.c"
    break;

  case 131: /* comOp: NOT IN  */
#line 768 "yacc_sql.y"
             { CONTEXT->comp = NOT_IN; }
#line 2308 "yacc_sql.tab.c"
    break;

  case 132: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 773 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2317 "yacc_sql.tab.c"
    break;


#line 2321 "yacc_sql.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (scanner, YY_("syntax error"));
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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

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


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 778 "yacc_sql.y"

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
