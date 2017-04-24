#pragma once
#include <fstream>
#include <iostream>
#include <iterator>


#include <vector>
#include "lexer_automaton.h"
#include "lexer_data.h"
#include "lexer_property_container.h"

namespace translator {

/// Lexer Automaton wrapper that operates on a file
class Lexer {
 public:
  Lexer(const PropertyContainer predefined_lexem = PropertyContainer())
      : m_predefined_lexem(predefined_lexem){};

  LexemData run(const std::string& filename) {
    std::ifstream file(filename);
    try {
      file.exceptions(file.failbit);
    } catch (const std::ifstream::failure& e) {
      std::cout << "File I/O Error" << e.what() << '\n';
      return LexemData();
    }
    if (worker != nullptr) {
      delete worker;
    }
    worker = new LexerAutomaton(file, m_predefined_lexem);
    return worker->run();
  }

  LexemData operator()(const std::string& filename) {
    return run(filename);
  }

  LexemData last_results() {
    if (worker == nullptr) {
      return LexemData();
    }
    return worker->data();
  }

 private:
  PropertyContainer m_predefined_lexem;
  LexerAutomaton *worker = nullptr;
};
}  // namespace translator
