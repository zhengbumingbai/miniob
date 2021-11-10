echo "Build Lex and Yacc."
cd ~/miniob/src/observer/sql/parser
flex --header-file="lex.yy.h" lex_sql.l
bison -d -b yacc_sql yacc_sql.y -v


echo "====================================================================="


echo "Compile Code."
cd ~/miniob/build/
make -j4