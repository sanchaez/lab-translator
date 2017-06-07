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
<logical-multipliers-list> --> AND <logical-multiplier><logical-multipliers-list> | <empty> 
<logical-multiplier> --> <expression><comparison-operator> <expression> |
                         [ <conditional-expression> ] | NOT <logical-multiplier> 
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
 --c
 <email> --> <identifier>@<string>.<string>
*/
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include "lexer.h"
#include "predefined_lexem.h"
#include "print_helpers.h"

using namespace translator;

#define STREQ(a, b) (strcmp((a), (b)) == 0)
#define INVALID_KEY 100
#define NO_INPUT 101
#define KEYERROR(keystr, reason)                                             \
  std::cout << "Wrong use of key " << keystr << ": " << reason << std::endl; \
  return INVALID_KEY;

inline void print_results(LexemData &results, std::ostream &output = std::cout) {
  output.width(7);
  output << "~~Lexem list\n";
  fixed_width_print_line({ ":name", ":id", ":row", ":column" }, 15, output);
  for (auto& x : results.tokens) {
    fixed_width_print_line({ x.name, std::to_string(x.symbol),
      std::to_string(x.row), std::to_string(x.column) },
      15, output);
  }
  output << "~~Lexem table\n";
  results.lexem_codes.print(output);
}

int main(int argc, char* argv[]) {
  Lexer lx(predefined_lexem);
  std::string input_file_name;
  std::string output_file_name;

  // parse command-line args
  // print help
  if (argc == 2 && STREQ(argv[1], "help")) {
    std::cout << "supported args:\
      help            - prints help(--help)\
      -f filename_in  - file to parse(--file)\
      -o filename_out - file to output(--output).Default is \"lexer_\" + filename_in \
      -v              - output to command line(--verbose)";
    return 0;
  }
  //parse rest
  std::string* pending = nullptr;
  bool use_std_cout = false;
  for (int i = 1; i < argc; ++i) {
    // if it's a key
    if (*(argv[i]) == '-') {
      if (pending) {
        // no file specified for the previous key
        // safe because 'pending' is set only after key
        KEYERROR(argv[i - 1], "No argument specified!")
      }
      if (STREQ(argv[i], "-f") || STREQ(argv[i], "--file")) {
        pending = &input_file_name;
      } else if (STREQ(argv[i], "-o") || STREQ(argv[i], "--output")) {
        pending = &output_file_name;
      } else if (STREQ(argv[i], "-v") || STREQ(argv[i], "--verbose")) {
        use_std_cout = true;
      } else {
        KEYERROR(argv[i], "Invalid key!")
      }
    } else {
      if (pending) {
        // write an argument
        *pending = argv[i];
        pending = nullptr;
      } else {
        // no key
        KEYERROR(argv[i], "Invalid key!")
      }
    }
  }
  if (pending) {
    KEYERROR(argv[argc - 1], "No argument specified!")
  }
  if (input_file_name.empty()) {
    std::cout << "No input specified!\n";
    return NO_INPUT;
  }
  // parse file
  auto results = lx(input_file_name);
  std::shared_ptr<std::ostream> output;
  if (output_file_name.empty()) {
    output_file_name = "lexer_" + input_file_name;
  } 
  output = std::make_shared<std::ofstream>(output_file_name.c_str());

  if (use_std_cout) {
    print_results(results);
  }
  print_results(results, *output);
  return 0;
}