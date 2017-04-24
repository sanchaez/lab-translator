#pragma once
#include <cctype>
#include <iostream>
#include "lexer_data.h"

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

  /// Operator overload that invokes main loop
  auto operator()() { return lexer_loop(m_file); }

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
    Exit
  } state;

  /// Read a char from a file
  ///-1 means eof
  void readchar(std::ifstream& file, char c) {
    if (!file.eof()) {
      c = file.get();
      if (c == '\r') {
        m_columns = 0;
      } else if (c == '\n') {
        ++m_rows;
        m_columns = 0;
      } else {
        ++m_columns;
      }
    } else
      c = -1;
  }

  /// Main lexer loop. Checks the symbols in file one by one and switches states
  LexemData lexer_loop(std::ifstream& file) {
    // TODO: clear previous run
    std::string input_buffer;
    char input_char;
    m_rows = 0;
    m_columns = 0;
    m_identifier_count = 1000;
    m_num_constant_count = 500;

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
          // expected eof
          if (input_char) {
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
            }
          } else {
            // expected eof
            state = LexerState::Exit;
          }
        } break;
        case LexerState::Identifier: {
          // read until not a letter or digit
          do {
            toupper(input_char);
            input_buffer += input_char;
            readchar(file, input_char);
          } while (isalnum(input_char));
          m_data.new_token(input_buffer, m_identifier_count, m_rows, m_columns);
          ++m_identifier_count;
          input_buffer.erase();
          state = LexerState::Input;
        } break;
        case LexerState::Delimiter: {
          // find a delimiter
          input_buffer += input_char;
          int code = m_data.lexem_codes[input_buffer];
          if (code) {
            m_data.new_token(input_buffer, code, m_rows, m_columns);
          } else {
            // read second char and search again
            readchar(file, input_char);
            if (!input_char) {
              state = LexerState::Error;
              break;
            }
            input_buffer += input_char;
            int code = m_data.lexem_codes[input_buffer];
            if (code) {
              m_data.new_token(input_buffer, code, m_rows, m_columns);
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
          } while (iswspace(input_char));
          state = LexerState::Input;
        } break;
        case LexerState::Number:
          // read while number
          {
            do {
              input_buffer += input_char;
              readchar(file, input_char);
            } while (isdigit(input_char));
            m_data.new_token(input_buffer, m_num_constant_count, m_rows,
                             m_columns);
            ++m_num_constant_count;
            state = LexerState::Start;
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
          if (!input_char) {
            state = LexerState::Error;
          } else {
            state = LexerState::EComment;
          }
        } break;
        case LexerState::EComment: {
          readchar(file, input_char);
          if (input_char) {
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
          std::cout << input_char << "(" << input_buffer.data() << ")"
                    << " at "
                    << "[" << m_rows << ", " << m_columns << "]\n";
          input_buffer.erase();
          readchar(file, input_char);
          state = LexerState::Input;
        } break;
        case LexerState::Exit:
          // void
          break;
      }
    }
    return m_data;
  }

  LexemData m_data;
  int m_rows = 0;
  int m_columns = 0;
  int m_identifier_count = 1000;
  int m_num_constant_count = 500;
  std::ifstream& m_file;
};

}  // namespace translator