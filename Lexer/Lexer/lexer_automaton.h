#pragma once
#include <cctype>
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
    Comment,
    Number,
    Error,
    Exit
  } state;

  /// Main lexer loop. Checks the symbols in file one by one and switches states
  LexemData lexer_loop(std::ifstream& file) {
    // TODO: clear previous run
    std::string input_buffer;
    char input_char;
    while (!file.eof()) {
      switch (state) {
        case LexerState::Start:
          state = LexerState::Input;
          break;
        case LexerState::Input:
          input_char = file.get();
          // check for ws
          if (iswspace(input_char)) {
            state = LexerState::Whitespace;
          } else if (isalpha(input_char)) {
            state = LexerState::Identifier;
          } else if (isdigit(input_char)) {
            state = LexerState::Number;
          } else if (input_char == '(') {
            state = LexerState::Comment;
            break;
          } else if (m_data.lexem_codes.find(input_char)) {
            //other character

          }
          break;
        case LexerState::Identifier:
          break;
        case LexerState::Delimiter:
          break;
        case LexerState::Whitespace:
          break;
        case LexerState::Number:
          break;
        case LexerState::Comment:
          break;
        case LexerState::Error:
          break;
        case LexerState::Exit:
          break;
      }
    }
  }

  LexemData m_data;

  std::ifstream& m_file;
};

}  // namespace translator