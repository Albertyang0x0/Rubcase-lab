file(REMOVE_RECURSE
  "../../../src/parser/lex.yy.cpp"
  "../../../src/parser/yacc.tab.cpp"
  "../../../src/parser/yacc.tab.h"
  "../../lib/libparser.a"
  "../../lib/libparser.pdb"
  "CMakeFiles/parser.dir/ast.cpp.o"
  "CMakeFiles/parser.dir/lex.yy.cpp.o"
  "CMakeFiles/parser.dir/yacc.tab.cpp.o"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/parser.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
