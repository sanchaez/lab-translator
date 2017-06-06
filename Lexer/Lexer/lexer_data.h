/* Basic declarations */
#pragma once
#include <unordered_map>
#include <vector>
#include "lexer_property_container.h"

namespace translator {
// Describes a token
struct Token {
  int symbol;
  std::string name;
  int row;
  int column;
};

// Holder for lexem array and property map
struct LexemData {
  LexemData(const PropertyContainer& default_lexem = PropertyContainer())
      : lexem_codes(default_lexem) {}

  std::vector<Token> tokens;
  PropertyContainer lexem_codes;

  void new_token(const std::string& lexem,
                 const int code,
                 const int row,
                 const int column) {
    if (lexem_codes[lexem] < 0) {
      lexem_codes.set(lexem, code);
    }
    tokens.push_back({code, lexem, row, column});
  }
};
}  // namespace translator