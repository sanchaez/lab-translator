// Lexer for SIGNAL 
// grammar:
// var 24
/*
<signal-program> --> <program>
<program> --> PROGRAM <procedure-identifier>;<block>.
<block> --> <variable-declarations> BEGIN <statements-list> END
<variable-declarations> --> VAR <declarations-list> | <empty>
<declarations-list> -- <declaration> <declarations-list> | <empty>
<declaration> --> <variable-identifier> : INTEGER ;
<statements-list> -- <declaration> <statements-list> | <empty>
<statements> --> <variable-identifier> := <conditional-expression> ;
<conditional-expression> --> <logical-summand> <logical>
<logical> --> OR <logical-summand> <logical> | <empty>
<logical-summand> --> <logical-multiplier> <logical-multipliers-list>
<logical-multipliers-list> --> AND <logical-multiplier>
<logical-multipliers-list> | <empty> <logical-multiplier> --> <expression>
<comparison-operator> <expression> | [ <conditional-expression> ] | NOT
<logical-multiplier> <comparison-operator> --> < | <= | = | <> | >= | >
<expression> --> <variable-identifier> | <unsigned-integer>
<variable-identifier> --> <identifier>
<procedure-identifier> --> <identifier>
<identifier> --> <letter><string>
<string> --> <letter><string> | <digit><string> | <empty>
<unsigned-integer> --> <digit><digits-sting>
<digits-string> --> <digit><digits-string> | <empty>
<digit> --> 0..9
<letter> --> A..Z
 --c
 <email> --> <identifier>@<string>.<string>
*/
#include <iostream>
#include <iomanip>
#include <string>
#include "lexer.h"
#include "predefined_lexem.h"
#include "print_helpers.h"

using namespace translator;

int main() {
  Lexer lx(predefined_lexem);
  auto results = lx("test.txt");
  std::cout.width(7);
  std::cout << "~~Lexem list\n";
  fixed_width_print_line({ ":name", ":id", ":row", ":column" });
  for (auto &x : results.tokens) {
    fixed_width_print_line({ x.name, std::to_string(x.symbol), std::to_string(x.row), std::to_string(x.column) });
  }
  std::cout << "~~Lexem table\n";
  results.lexem_codes.print();
  std::cin.get();
  return 0;
}