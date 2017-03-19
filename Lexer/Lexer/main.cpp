// Lexer for SIGNAL 
// grammar:
// var 24
/*
 <signal-program> --> <program>
 <program> --> PROGRAM <procedure-identifier>;<block>.
 <block> --> <variable-declarations> BEGIN <statements-list> END
 <variable-declarations> --> VAR <declarations-list> | <empty>
 <declarations-list> -- <declaration> <declarations-list> | <empty>
 <declaration> --> <variable-identifier> := <conditional-expression> ;
 <conditional-expression> --> <logical-summand> <logical>
 <logical> --> OR <logical-summand> <logical> | <empty>
 <logical-summand> --> <logical-multiplier> <logical-multipliers-list>
 <logical-multipliers-list> --> AND <logical-multiplier> <logical-multipliers-list> | <empty>
 <logical-multiplier> --> <expression> <comparison-operator> <expression> | [ <conditional-expression> ] 
      | NOT <logical-multiplier>
 <comparison-operator> --> < | <= | = | <> | >= | >
 <expression> --> <variable-identifier> | <unsigned-integer>
 <variable-identifier> --> <identifier>
 <procedure-identifier> --> <identifier>
 <identifier> --> <letter><string>
 <string> --> <letter><string> | <digit><string> | <empty>
 <unsigned-integer> --> <digit><digits-sting>
 <digits-string> --> <digit><digits-string> | <empty>
 <digit> --> 0..9
 <letter> --> A..Z
*/

int main() {
  return 0;
}