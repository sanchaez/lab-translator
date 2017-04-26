#pragma once
#include <cctype>
#include <iostream>
#include <string>
#include "lexer_data.h"

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
*/

class PropertyContainer;

namespace translator {

/// Lexer Automaton
/// This automaton acts like functor saving the file stream reference and data
/// created in the process.
class LexerAutomaton {
 public:
  /// Lexer Automaton constructor

  LexerAutomaton(std::ifstream& file,
                 PropertyContainer predefined_lexem = PropertyContainer())
      : m_file(file), m_data(predefined_lexem) {}
  /// Invokes the main loop
  auto run() { return lexer_loop(m_file); }
  /// Operator overload that invokes main loop
  auto operator()() { return run(); }

  /// Get last data
  LexemData data() const { return m_data; }

 private:
  /// Describes lexer states.
  enum LexerState {
    Start,
    Input,
    Identifier,
    Delimiter,
    Whitespace,
    BComment,
    EComment,
    Comment,
    Number,
    Error,
    Exit,
    //custom
    EmailWebsiteName,
    EmailEnd
  } state;

  /// Read a char from a file
  ///-1 means eof
  void readchar(std::ifstream& file, char& c) {
    try {
      c = file.get();
    } catch (const std::ifstream::failure&) {
      c = -1;
      return;
    }
    if (c == '\r') {
      m_column = 0;
    } else if (c == '\n') {
      ++m_row;
      m_column = 0;
    } else {
      ++m_column;
    }
  }

  /// Main lexer loop. Checks the symbols in file one by one and switches states
  LexemData lexer_loop(std::ifstream& file) {
    // TODO: clear previous run
    std::string input_buffer;
    char input_char = -1;
    m_row_start = 0;
    m_column_start = 0;
    m_identifier_count = 1000;
    m_num_constant_count = 500;
    state = LexerState::Start;
    while (state != LexerState::Exit) {
      switch (state) {
        case LexerState::Start: {
          if (file.eof()) {
            state = LexerState::Exit;
            break;
          }
          readchar(file, input_char);
          state = LexerState::Input;
        } break;
        case LexerState::Input: {
          m_column_start = m_column;
          m_row_start = m_row;
          // expected eof
          if (input_char < 0) {
            // expected eof
            state = LexerState::Exit;
          } else {
            // process char
            if (iswspace(input_char)) {
              state = LexerState::Whitespace;
            } else if (isalpha(input_char)) {
              state = LexerState::Identifier;
            } else if (isdigit(input_char)) {
              state = LexerState::Number;
            } else if (input_char == '(') {
              state = LexerState::Comment;
            } else if (m_data.lexem_codes.isallowed(input_char)) {
              // other character
              state = LexerState::Delimiter;
            } else {
              state = LexerState::Error;
            }
          }
        } break;
        case LexerState::Identifier: {
          // read until not a letter or digit
          do {
            toupper(input_char);
            input_buffer += input_char;
            readchar(file, input_char);
          } while ((input_char >= 0) && isalnum(input_char));
          //custom
          if (input_char == '@') {
            state = LexerState::EmailWebsiteName;
            break;
          }
          // find identifier in predefined list"
          int identifier_code = m_data.lexem_codes[input_buffer];
          if (identifier_code < 0) {
            identifier_code = m_identifier_count++;
          }
          m_data.new_token(input_buffer, identifier_code, m_row_start, m_column_start);
          input_buffer.erase();
          state = LexerState::Input;
        } break;
        case LexerState::Delimiter: {
          // find a delimiter
          input_buffer += input_char;
          int code = m_data.lexem_codes[input_buffer];
          if (code >= 0) {
            m_data.new_token(input_buffer, code, m_row_start, m_column_start);
          } else {
            // read second char and search again
            readchar(file, input_char);
            if (input_char < 0) {
              state = LexerState::Error;
              break;
            }
            input_buffer += input_char;
            int code = m_data.lexem_codes[input_buffer];
            if (code) {
              m_data.new_token(input_buffer, code, m_row_start, m_column_start);
            } else {
              state = LexerState::Error;
              break;
            }
          }
          input_buffer.erase();
          state = LexerState::Start;
        } break;
        case LexerState::Whitespace: {
          // read while whitespace
          do {
            readchar(file, input_char);
          } while ((input_char >= 0) && iswspace(input_char));
          state = LexerState::Input;
        } break;
        case LexerState::Number:
          // read while number
          {
            do {
              input_buffer += input_char;
              readchar(file, input_char);
            } while ((input_char >= 0) && isdigit(input_char));
            int num_constant_code = m_data.lexem_codes[input_buffer];
            if (num_constant_code < 0) {
              num_constant_code = m_num_constant_count++;
            }
            m_data.new_token(input_buffer, m_num_constant_count, m_row_start,
                             m_column_start);
            input_buffer.erase();
            state = LexerState::Input;
          }
          break;
        case LexerState::BComment: {
          // get next char
          readchar(file, input_char);
          if (input_char == '*' && !file.eof()) {
            readchar(file, input_char);
            state = LexerState::Comment;
          } else {
            state = LexerState::Error;
          }
        } break;
        case LexerState::Comment: {
          while (input_char != '*' && input_char) {
            readchar(file, input_char);
          }
          if (input_char < 0) {
            state = LexerState::Error;
          } else {
            state = LexerState::EComment;
          }
        } break;
        case LexerState::EComment: {
          readchar(file, input_char);
          if (input_char >= 0) {
            if (input_char != ')') {
              state = LexerState::Comment;
            } else {
              state = LexerState::Start;
            }
          } else {
            state = LexerState::Error;
          }
        } break;
        case LexerState::Error: {
          // TODO: process errors the good way
          std::cout << "Lexer error:";
          if (file.eof()) {
            std::cout << "Unexpected end of file. \n";
          } else {
            std::cout << "Unknown identifier. \n";
          }
          std::cout << input_char << "(" << input_buffer << ")"
                    << " at "
                    << "[" << m_row_start << ", " << m_column_start << "]\n";
          input_buffer.erase();
          readchar(file, input_char);
          state = LexerState::Input;
        } break;
        case LexerState::Exit:
          // void
          break;
        //custom
        case LexerState::EmailWebsiteName:
          //read until dot or eof
          do {
            input_buffer += input_char;
            readchar(file, input_char);
          } while ((input_char >= 0) && isalnum(input_char) && input_char != '.');

          if (input_char == '.') {
            state = LexerState::EmailEnd;
          } else {
            state = LexerState::Error;
          }
          break;
        case LexerState::EmailEnd:
        {
          do {
            input_buffer += input_char;
            readchar(file, input_char);
          } while ((input_char >= 0) && isalnum(input_char));

          int email_code = m_data.lexem_codes[input_buffer];
          if (email_code < 0) {
            email_code = m_email_count++;
          }

          m_data.new_token(input_buffer, email_code, m_row_start,
            m_column_start);
          input_buffer.erase();

          if (input_char < 0) {
            state = LexerState::Exit;
          } else {
            state = LexerState::Input;
          }

        } break;
      }
    }
    return m_data;
  }

  LexemData m_data;
  int m_row_start = 0;
  int m_column_start = 0;
  int m_row = 0;
  int m_column = 0;
  int m_identifier_count = 1000;
  int m_num_constant_count = 500;
  //custom 
  int m_email_count = 2000;
  std::ifstream& m_file;
};  // namespace translator

}  // namespace translator