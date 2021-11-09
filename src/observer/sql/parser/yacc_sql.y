
%{

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

%}

%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        INDEX
        SELECT
        DESC
        ASC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        DATE_T
        TEXT_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        SET
        ON
        LOAD
        DATA
        INFILE
        EQ
        LT
        GT
        LE
        GE
        NE
		AGGR_COUNT
		AGGR_MAX
		AGGR_MIN
		AGGR_AVG
        UNIQUE //zt UNIQUE
		NOT
		NULLTOKEN
		NULLABLE
		ISTOKEN
        ORDER
        GROUP
        BY
		INNER
		JOIN
		ADD_OP
		SUB_OP
		// MUL_OP
		DIV_OP

%union {
  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  struct _ExpressionTree *exp_tree;
  struct _ExpressionNode *exp_node;
  char *string;
  int number;
  float floats;
  char *position;
}

%token <number> NUMBER
%token <floats> FLOAT 
%token <string> ID
// 表示 DATE是一个字符串
%token <string> DATE
%token <string> PATH
%token <string> SSS
%token <string> STAR
%token <string> STRING_V
//非终结符

%type <number> type;
%type <condition1> condition;
%type <value1> value;
%type <number> number;
%type <number> order_type
%type <number> aggr_op
%type <number> add_or_sub
%type <number> sign
%type <number> mul_or_div
%type <exp_node> atom_expression
%type <exp_node> add_sub_expression
%type <exp_node> mul_div_expression

%%

commands:		//commands or sqls. parser starts here.
    /* empty */
    | commands command
    ;

command:
	  select  
	| insert
	| update
	| delete
	| create_table
	| drop_table
	| show_tables
	| desc_table
	| create_index	
	| drop_index
	| sync
	| begin
	| commit
	| rollback
	| load_data
	| help
	| exit
    ;

exit:			
    EXIT SEMICOLON {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, $3);
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, $2);
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE index_type INDEX ID ON ID LBRACE index_attr_id id_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $4, $6);
		}
    ;

index_type:
    /* empty */
    | UNIQUE {
        create_index_unique_init(&CONTEXT->ssql->sstr.create_index);
    }

id_list:
    /* empty */
    | COMMA index_attr_id id_list

index_attr_id:
    ID {
        create_index_attr_init(&CONTEXT->ssql->sstr.create_index,$1);
    }


drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, $3);
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE 
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
    |ID_get type
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
	|ID_get type NOT NULLTOKEN {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
	|ID_get type NULLABLE {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
	}
    ;
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
       | FLOAT_T { $$=FLOATS; }
    //    新增DATE_T 属性的token
       | DATE_T { $$=DATES; }
       | TEXT_T {$$=TEXTS;}
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES record record_list SEMICOLON 
		{
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->records, CONTEXT->record_length);

      //临时变量清零
      CONTEXT->value_length=0;
      CONTEXT->record_length=0;
    }
    
record_list:
    /* empty */
    | COMMA record record_list

record:
    LBRACE value value_list RBRACE {
        record_init(&CONTEXT->records[CONTEXT->record_length++],CONTEXT->values,CONTEXT->value_length);
        CONTEXT->value_length = 0;
    }
    
value_list:
    /* empty */
    | COMMA value value_list  { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
    ;
value:
    sign NUMBER{
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], $2,$1);
		}
    |sign FLOAT{
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], $2,$1);
		}
    |SSS {
			$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
        // 新增value类型 DATE的字符串处理
    |DATE {
			$1 = substr($1,1,strlen($1)-2);
  		value_init_date(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
	|NULLTOKEN {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
    ;

sign:
    /* empty */ {
        $$ = ADD;
    }
    | add_or_sub {
        $$ = $1;
    }
    ;

delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, $3);
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, $2, $4, value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
    ;
select:				/*  select 语句的语法解析树*/
    SELECT select_attr FROM ID rel_list inner_join where group order SEMICOLON
		{
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
    ;

order:
    /* empty */ 
    | ORDER BY ID order_type order_list {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, $4, NULL, $3);
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }
    | ORDER BY ID DOT ID order_type order_list {	
            OrderAttr order_attr;
            order_attr_init(&order_attr, $6, $3, $5);
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
        }        
    ;
group:
	/* empty */ 
    | GROUP BY ID group_id_list {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, $3);
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
    | GROUP BY ID DOT ID group_id_list {	
		GroupAttr group_attr;
		group_attr_init(&group_attr, $3, $5);
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}        
    ;

group_id_list:
	/* empty */
	| COMMA ID group_id_list {
		GroupAttr group_attr;
		group_attr_init(&group_attr, NULL, $2);
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
	| COMMA ID DOT ID group_id_list {
		GroupAttr group_attr;
		group_attr_init(&group_attr, $2, $4);
		selects_append_group_attribute(&CONTEXT->ssql->sstr.selection, &group_attr);
	}
    ;

order_list:
    /* empty */
    | COMMA ID order_type order_list {
            OrderAttr order_attr;
            order_attr_init(&order_attr, $3, NULL, $2);
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
    | COMMA ID DOT ID order_type order_list {
            OrderAttr order_attr;
            order_attr_init(&order_attr, $5, $2, $4);
            selects_append_order_attribute(&CONTEXT->ssql->sstr.selection, &order_attr);
      }
  	;

order_type:
    /* empty */ {
        $$ = ASC_T; // 默认升序
    }
    | ASC {
        $$ = ASC_T; //上一行的ASC表示token 这一行的ASC表示的是enum里的类型
    }
    | DESC {
        $$ = DESC_T; // 
    }
    ;

select_attr:
    STAR {  
        RelAttr attr;
        relation_attr_init(&attr, NULL, "*", NULL);
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
    |
  	attr attr_list {

    }
    ;

attr:
    add_sub_expression {
        RelAttr attr;
        relation_attr_init(&attr, NULL, NULL, $1);
        selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
    }
    | aggr {

    }
    ;

attr_list: 
    /* EMPTY */
    | COMMA attr attr_list {

    }
    ;


/* LR 分析法 忘得差不多了 百度的结果如下： */
add_sub_expression:
    add_sub_expression add_or_sub mul_div_expression {
        $$=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init($$, 1, $1, $2, $3, 0, NULL, NULL, 0);
    }
    | mul_div_expression {
        $$ = $1;
    }
    ;

add_or_sub:
    ADD_OP {
        $$ = ADD;
    }
    | SUB_OP {
        $$ = SUB;
    }
    ;

mul_div_expression:
    mul_div_expression mul_or_div atom_expression {
        $$=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        expression_node_init($$, 1, $1, $2, $3, 0, NULL, NULL, 0);
    } 
    | atom_expression {
        $$ = $1;
    }
    ;

mul_or_div:
    STAR {
        /* 记录操作符 */
        $$ = MUL;
    }
    | DIV_OP {
        /* 记录操作符 */
        $$ = DIV;
    }
    ;

atom_expression:
    LBRACE add_sub_expression RBRACE{
        $$ = $2;
        $$->isBracket = 1;
    }
    |
	ID {
        /* 记录 列名 */
        $$=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, NULL, $1, NULL);
        expression_node_init($$, 0, NULL, 0, NULL, 0, attr, NULL, 0);
	}
    |
    ID DOT ID {
        /* 记录表名 */
        $$=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        RelAttr *attr = ( RelAttr *)malloc(sizeof( RelAttr));;
        relation_attr_init(attr, $1, $3, NULL);
        expression_node_init($$, 0, NULL, 0, NULL, 0, attr, NULL, 0);
	}
    | value {
        $$=( ExpressionNode *)malloc(sizeof( ExpressionNode));
        Value *value_ = &CONTEXT->values[CONTEXT->value_length - 1];
        expression_node_init($$, 0, NULL, 0, NULL, 1, NULL, value_, 0);
    }
	;

aggr:
	aggr_op LBRACE STAR RBRACE {
            ExpressionNode node;
            RelAttr rel_attr;
            relation_attr_init(&rel_attr,NULL,"*",NULL);
            expression_node_init(&node,0,NULL,0,NULL,0,&rel_attr,NULL,0 );
			AggrAttr attr;
			/*第二个参数refer to AggrType */
			aggr_attr_init(&attr, $1, &node);
			selects_append_aggr_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| aggr_op LBRACE atom_expression RBRACE{
            ExpressionNode *node = $3;
            AggrAttr attr;
			/*第二个参数refer to AggrType */
			aggr_attr_init(&attr, $1, node);
			selects_append_aggr_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
    ;

aggr_op:
	AGGR_COUNT { $$=1; }
	| AGGR_AVG  { $$=2; }
	| AGGR_MAX { $$=3; }
	| AGGR_MIN { $$=4; }
	;


rel_list:
    /* empty */
    | COMMA ID rel_list {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
		  }
    ;
inner_join:
	/* empty */
	| INNER JOIN ID rel_list on inner_join {
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $3);
		}
	;
on:
	/* empty */
	| ON condition condition_list {

		}
	;


where:
    /* empty */ 
    | WHERE condition condition_list {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition_list:
    /* empty */
    | AND condition condition_list {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition:
    add_sub_expression comOp add_sub_expression {
        Condition condition;
        condition_init(&condition, CONTEXT->comp, 0, NULL, NULL, 0, NULL, NULL, $1, $3);
        CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    }
    ;

comOp:
  	  EQ { CONTEXT->comp = EQUAL_TO; }
    | LT { CONTEXT->comp = LESS_THAN; }
    | GT { CONTEXT->comp = GREAT_THAN; }
    | LE { CONTEXT->comp = LESS_EQUAL; }
    | GE { CONTEXT->comp = GREAT_EQUAL; }
    | NE { CONTEXT->comp = NOT_EQUAL; }
	| ISTOKEN { CONTEXT->comp = IS; }
	| ISTOKEN NOT { CONTEXT->comp = ISNOT; }
    ;

load_data:
		LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON
		{
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, $7, $4);
		}
		;
%%
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
