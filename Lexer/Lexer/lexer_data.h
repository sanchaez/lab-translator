/* Basic declarations */
#pragma once
#include <unordered_map>
#include <vector>
#include "lexer_property_container.h"

namespace translator {

// Describes a token
struct LexemToken {
  LexemToken() : symbol(-1), name(""), row(-1), column(-1) {}
  LexemToken(const int _symbol,
             const std::string& _name,
             const int _row,
             const int _column)
      : symbol(_symbol), name(_name), row(_row), column(_column) {}
  int symbol;
  std::string name;
  int row;
  int column;
};

// Holder for lexem array and property map
struct LexemData {
  LexemData(const PropertyContainer& default_lexem = PropertyContainer())
      : lexem_codes(default_lexem) {}

  std::vector<LexemToken> tokens;
  PropertyContainer lexem_codes;

  void new_token(const std::string& lexem,
                 const int code,
                 const int row,
                 const int column) {
    if (lexem_codes[lexem] < 0) {
      lexem_codes.set(lexem, code);
    }
    tokens.emplace_back(code, lexem, row, column);
  }
};
}  // namespace translator