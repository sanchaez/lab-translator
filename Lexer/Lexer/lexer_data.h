/* Basic declarations */
#pragma once
#include <unordered_map>
#include <vector>

class PropertyContainer;

namespace translator {
// Describes a token
struct Token {
  int symbol;
  int row;
  int column;
};

// Holder for lexem array and property map
struct LexemData {
  LexemData(const PropertyContainer& default_lexem = PropertyContainer())
      : lexem_codes(default_lexem), data() {
  
  }

  std::vector<Token> data;
  PropertyContainer lexem_codes;
};
}  // namespace translator