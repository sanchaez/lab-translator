#pragma once
#include <fstream>
#include <iostream>
#include <iterator>

#include <vector>
#include "lexer_automaton.h"
#include "lexer_data.h"
#include "lexer_property_container.h"
#include "predefined_lexem.h"

namespace translator {

/// Lexer Automaton wrapper that operates on a file
class Lexer {
 public:
  Lexer(const PropertyContainer predefined_lexem = g_predefined_lexem)
      : m_predefined_lexem(predefined_lexem){};

  LexemData operator()(const std::string& filename) {
    std::ifstream file(filename);
    try {
      file.exceptions(file.failbit);
    } catch (const std::ifstream::failure& e) {
      std::cout << "Caught an ifstream::failure.\n"
                << "Explanatory string: " << e.what() << '\n'
                << "Error code: " << e.code() << '\n';
      return LexemData();
    }
    LexerAutomaton worker(file, m_predefined_lexem);
    return worker();
  }

 private:
  PropertyContainer m_predefined_lexem;
};
}  // namespace translator
